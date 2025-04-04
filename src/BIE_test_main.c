#include "snn_library.h"
#include "load_data.h"
#include "helpers.h"
#include "training_rules/stdp.h"

#include "neuron_models/lif_neuron.h"
//#include "neuron_models/GPU_lif_neuron.cuh"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../MNIST_for_C/mnist.h"
#include "encoders/image_encoders.h"


/* main.c */
int main(int argc, char *argv[]) {
    // variables definition
    
    int simulation_type, execution_type, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses, n, m, repetitions, time_steps, time_step;

    int **synaptic_connections, *neuron_excitatory, *training_zones; // change to uint in the future
    int *delay_list;
    float *weight_list;

    void *neuron_initializer; // function to initialize neurons
    void (*input_step)(), (*output_step)(); // functions to run a step on the simulation

    int i,j,l;

    n = strtoul(argv[1], NULL, 10);
    m = strtoul(argv[2], NULL, 10);

    execution_type = strtoul(argv[3], NULL, 10);
    neuron_type = strtoul(argv[4], NULL, 10);
    simulation_type = strtoul(argv[5], NULL, 10);

    spiking_nn_t *networks;
    networks = (spiking_nn_t *)malloc(n * sizeof(spiking_nn_t));

    char **network_files;
    network_files = malloc(n * sizeof(char *));
    
    // store network file names
    for(i = 0; i<n; i++){
        network_files[i] = argv[i+6];
    }

    // load and initialize networks
    for(i = 0; i<n; i++){
        n_neurons = 0;
        n_input = 0;
        n_output = 0;
        n_synapses = 0;
        n_input_synapses = 0;
        n_output_synapses = 0;

        load_network_information(network_files[i], &n_neurons, &n_input, &n_output, &n_synapses, &n_input_synapses, &n_output_synapses,
            &synaptic_connections, &neuron_excitatory, &weight_list, &delay_list, &training_zones);

        initialize_network(&networks[i], neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses, 
            neuron_excitatory, synaptic_connections, weight_list, delay_list, training_zones);

        //for(j=0; j<networks[i].n_neurons + 1; j++){
        //    free(&(synaptic_connections[j]));
        //}
        free(synaptic_connections);
        free(weight_list);
        free(neuron_excitatory);
        free(delay_list);
        free(training_zones);
    }
    

    /*printf("\n============\nPrinting networks information\n==========\n");

    for(i = 0; i<n; i++){
        printf("Network:\n");
        printf(" - Neurons: %d\n", networks[i].n_neurons);
        printf(" - Input neurons: %d\n", networks[i].n_input);
        printf(" - Output neurons: %d\n", networks[i].n_output);
        printf(" - Synapses: %d\n", networks[i].n_synapses);
        printf(" - Input synapses: %d\n", networks[i].n_input_synapses);
        printf(" - Output synapses: %d\n", networks[i].n_output_synapses);

        int correct = 1;
        for(j = 0; j<networks[i].n_synapses; j++){
            printf("%d ", networks[i].synapses[j].max_spikes);
            if(networks[i].synapses[j].max_spikes == 150 && j >= 784){
                correct = 0;
            }
        }
        printf("\n");
        if(correct == 0) printf(" - Correct: No\n");
    }*/

    // Initialize step function depending on neuron type
    switch (neuron_type)
    {
        case 0: // LIF
            //step = &step_lif_neuron;
            input_step = &lif_neuron_compute_input_synapses;//&lif_neuron_compute_input_synapses;
	        output_step = &lif_neuron_compute_output_synapses;
	    break;
        case 1: // HH
            break;
        default:
            break;
    }

    // randomize execution
    srand(time(NULL));

    
    // load dataset
    load_mnist(); // load information from mnist dataset

    spike_images_t spike_images;
    
    // information about the dataset
    int pT = 100;
    int pDT = 1;
    int bins = (int)(pT/pDT);

    spike_images.bins = bins;
    spike_images.image_size = 784;
    spike_images.n_images = 5000;

    // allocate memory for spiking images
    spike_images.spike_images = malloc(spike_images.n_images * sizeof(spike_image_t)); // alloc memory for images
    for(i = 0; i<spike_images.n_images; i++){
        spike_images.spike_images[i].spike_image = malloc(spike_images.image_size * sizeof(int *));
        for(j=0; j<spike_images.image_size; j++)
            spike_images.spike_images[i].spike_image[j] = malloc(spike_images.bins * sizeof(int));
    }

    // load spike trains
    FILE *f = fopen(argv[11], "r");
    if(f == NULL) printf("Error opening the file\n");

    printf("Reading input spike train...\n");
    for(i=0; i<spike_images.n_images; i++){
        for(j=0; j<spike_images.image_size; j++){
            fscanf(f, "%d", &(spike_images.spike_images[i].spike_image[j][0]));
            for(l = 1; l<spike_images.spike_images[i].spike_image[j][0]; l++){
                fscanf(f, "%d", &(spike_images.spike_images[i].spike_image[j][l]));
            }
        }
    }

    printf("Input spike trains readed!\n");


    // alloc memory to store spike trains for each network: for each network, for each neuron, bins * 2 spikes
    time_steps = bins * 2;
    unsigned char ***generated_spikes = malloc(n * sizeof(unsigned char **));
    for(i=0; i<n; i++){
        generated_spikes[i] = malloc(networks[i].n_neurons * sizeof(unsigned char *));
        for (j = 0; j<n_neurons; j++)
            generated_spikes[i][j] = malloc(time_steps * sizeof(unsigned char));
    }
    


    // BIE computation

    // Pi: sum of spikes generated per neuron, P: list of Pi-s
    repetitions = 100;
    int ****P = malloc(repetitions * sizeof(int ***));
    for(int r; r<repetitions; r++){
        P[r] = malloc(n * sizeof(int **));
        for(i = 0; i<n; i++){
            P[r][i] = malloc(m * sizeof(int *));
            for(j = 0; j<m; j++)   
                P[r][i][j] = malloc(networks[i].n_neurons * sizeof(int));
        }
    }

    // Prepare structure to store results

    printf("\n=============\nStarting simulation\n================\n");

    // Compute BIE
    int *sample_indexes = malloc(m * sizeof(int));
    for(i = 0; i < repetitions; i++){
        printf("Running repetition %d\n", i);
        // select m sample indexes
        int n_selected_samples = 0;
        for(j = 0; j<m; j++){
            // check that the sample has not been selected
            //for(l = 0; l<n_selected_samples; l++){

            //}
            sample_indexes[j] = rand() % spike_images.n_images; 
        }

        // compute for each network
        for(j = 0; j<n; j++){
            printf(" - Computing network %d\n", j);
            for(l = 0; l<m; l++){ // all selected samples
                printf(" -- Computing sample %d\n", l);
                int sample_index = sample_indexes[l];

                // Simulate sample on network j
                
                // initialize lif neurons of network j
                #pragma omp parallel for schedule(dynamic, 10) private(s)  // this and the next loops: 4.5 seconds
                for(int s = 0; s<networks[j].n_neurons; s++){
                    re_initialize_lif_neuron(&(networks[j]), s);
                }
                //printf("Neuron reinitialized\n");
                // reinitialize synapses

                // initialize synapses of network j
                #pragma omp parallel for schedule(dynamic, 10) private(s) 
                for(int s=0; s<networks[j].n_synapses; s++){
                    re_initialize_synapse(&(networks[j].synapses[s]));
                }
                //printf("Synapses reinitialized\n");

                // introduce sample spikes into the network
                // introduce spikes into input neurons input synapses (I think that this is not paralelizable)
                for(int s=0; s<networks[j].n_input; s++){ // 3 seconds??
                    synapse_t *synap = &(networks[j].synapses[networks[j].lif_neurons[s].input_synapse_indexes[0]]);
                    for(int k = 0; k<spike_images.spike_images[sample_index].spike_image[s][0]; k++){ // introduce spike image s to s-th neuron
                        synap->l_spike_times[synap->last_spike] = spike_images.spike_images[sample_index].spike_image[s][k];
                        synap->last_spike +=1;
                    }
                }
                //printf("Spikes introduced on input neurons\n");
            
                for(int s=0; s<bins; s++){ // this loop more or less 6 
                    #pragma omp parallel num_threads(n_process)
                    {
                        #pragma omp for schedule(dynamic, 10) private(k) 
                        for(int k=0; k<networks[j].n_neurons; k++){
                            input_step(&(networks[j]), s, k, generated_spikes[j]); 
                        } 
                        //printf("Input synapses computed\n");

                        #pragma omp for schedule(dynamic, 10) private(k)
                        for(int k=0; k<networks[j].n_neurons; k++){
                            int a = 0;
                            output_step(&(networks[j]), s, k, generated_spikes[j], &(P[i][j][l][k])); // P[j][l], network j, sample l
                        }   
                        
                        // not learning on BIE evaluation
                        /*#pragma omp for schedule(dynamic, 50) private(k)
                        for(int k  = 0; k<networks[j].n_synapses; k++){
                            networks[j].synapses[i].learning_rule(&(networks[j].synapses[l])); 
                        }*/
                    }
                }
            }
        }
    }



    return 0;

/*    FILE *output_file;
    output_file = fopen(argv[7], "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");
    fprintf(output_file, "%f ", total_elapsed_time);

    // print spikes generated by each neuron
   /* FILE *output_file;
    output_file = fopen(argv[7], "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");
    for (int i = 0; i<n_neurons; i++)
    {
        for(int j = 0; j<time_steps; j++){
            fprintf(output_file, "%c", generated_spikes[i][j]);
        }
        fprintf(output_file, "\n");
    }

    // print weight values at last
    /*FILE *output_file2;
    output_file2 = fopen(argv[8], "w");
    if(output_file2 == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");
    for (int i = 0; i<n_synapses; i++)
    {
        fprintf(output_file2, "%f ", snn.synapses[i].w);
        fprintf(output_file2, "\n");
    }

    FILE *output_file3 = fopen(argv[9], "w");
    if(output_file3 == NULL) printf("Error opening the file\n");
    /*fprintf(output_file3, "%f ", elapsed_time);
    fprintf(output_file3, "%f ", elapsed_time_neurons);
    fprintf(output_file3, "%f \n", elapsed_time_synapses);
*/
    /*fclose(output_file);
    fclose(output_file2);
    fclose(output_file3);
    // free memory
//    free(snn.lif_neurons);
//    free(snn.synapses);*/

    //return 0;
}
