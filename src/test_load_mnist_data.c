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
    int execution_type, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses;
    int *motif_list;
    int simulation_type, time_steps, time_step=0;

    //__int8_t *synapse_matrix;
    int **synaptic_connections, *neuron_excitatory, *training_zones; // change to uint in the future
    int *delay_list;
    double *weight_list;

    void *neuron_initializer; // function to initialize neurons
    void (*input_step)(), (*output_step)(); // functions to run a step on the simulation

    int i, j, l;

    // Load information from input parameters
    execution_type = strtoul(argv[1], NULL, 10);
    neuron_type = strtoul(argv[2], NULL, 10);
    simulation_type = strtoul(argv[3], NULL, 10);

    // randomize execution
    srand(time(NULL));

    // load information about the snn from file
    load_network_information(argv[4], &n_neurons, &n_input, &n_output, &n_synapses, &n_input_synapses, &n_output_synapses,
                            &synaptic_connections, &neuron_excitatory, &weight_list, &delay_list, &training_zones);


    time_steps = strtoul(argv[5], NULL, 10);

    // initialize snn structure
    spiking_nn_t snn;
    initialize_network(&snn, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses, 
                        neuron_excitatory, synaptic_connections, weight_list, delay_list, training_zones);


    printf("Network initialized\n");
    // free memory
    //for(i=0; i<(n_neurons+1); i++){
    //    free(synaptic_connections[i]);
    //}
    free(synaptic_connections);
    free(weight_list);
    free(delay_list);
    free(training_zones);


    //load_input_spike_trains_on_snn(argv[6], &snn);
    load_mnist(); // load information from mnist dataset

    spike_images_t spike_images;
    
    int pT = 100;
    int pDT = 1;
    int bins = (int)(pT/pDT);

    spike_images.bins = bins;
    spike_images.image_size = 784;
    spike_images.n_images = 5000;


    spike_images.spike_images = malloc(spike_images.n_images * sizeof(spike_image_t)); // alloc memory for images
    for(i = 0; i<spike_images.n_images; i++){
        spike_images.spike_images[i].spike_image = malloc(spike_images.image_size * sizeof(int *));
        for(j=0; j<spike_images.image_size; j++)
            spike_images.spike_images[i].spike_image[j] = malloc(spike_images.bins * sizeof(int));
    }

    // load spike trains
    FILE *f = fopen(argv[6], "r");
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

    /*printf("Converting images to spike trains...\n");

    for(i=0; i<spike_images.n_images; i++){
        // alloc memory for image pixels
        spike_images.spike_images[i].spike_image = malloc(spike_images.image_size * sizeof(int *)); // each image has 728 pixels (this must be generalized)

        for(j=0; j<spike_images.image_size; j++){ // alloc bins integers for each pixel spike train --> this must be corrected, as bins is used for 0,1 spike trains, but I use spike times, so less positions are needed
            spike_images.spike_images[i].spike_image[j] = malloc(bins * sizeof(int));
        }
    }

    printf("Memory reserved!\n");

    // convert images into spike images
    convert_images_to_spikes_by_poisson_distribution(&spike_images, train_image, spike_images.n_images, spike_images.image_size, spike_images.bins);

    printf("Conversion correctly completed!\n");*/


/*    for(i = 0; i<bins; i++){
        printf("%d ",spike_images.spike_images[5].spike_image[70][i]);
    }
*/
    // variable to store generated spikes
    unsigned char **generated_spikes = malloc(n_neurons * sizeof(unsigned char *));
    for (i = 0; i<n_neurons; i++)
        generated_spikes[i] = malloc(time_steps * sizeof(unsigned char));


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

    // load amount of threads 
    int n_process = strtoul(argv[10], NULL, 10);


    struct timespec start, end;
    struct timespec start_neurons, end_neurons;
    struct timespec start_synapses, end_synapses;
    struct timespec start_bin, end_bin;

    double elapsed_time_neurons= 0;
    double elapsed_time_synapses = 0;

    double elapsed_time, input_neurons_elapsed_time=0, output_neurons_elapsed_time=0, synapses_elapse_time=0;

    struct timespec total_start, total_end;
    clock_gettime(CLOCK_MONOTONIC, &total_start);

    // simulation / training
    if(execution_type == 0){ // clock
        while(time_step < time_steps){
            // process all neurons
            #pragma omp parallel num_threads(n_process)
            {
                #pragma omp for schedule(dynamic, 10) private(i) 
                for(int i=0; i<snn.n_neurons; i++){
                    input_step(&snn, time_step, i, generated_spikes);
                } 

                #pragma omp for schedule(dynamic, 10) private(i)
                for(int i=0; i<snn.n_neurons; i++){
                output_step(&snn, time_step, i, generated_spikes);
                }

                // stdp
                #pragma omp for schedule(dynamic, 50) private(i)
                for(int i = 0; i<snn.n_synapses; i++){
                    snn.synapses[i].learning_rule(&(snn.synapses[i])); 
                }
            }
            
            time_step++;
            
            if(time_step % 1000 == 0)
                printf("Time step: %d/%d\n", time_step, time_steps);  
        }
    } // model training
    else{
        // for each sample on the dataset
        for(i = 0; i<spike_images.n_images; i++){
            printf("Starting processing sample %d...\n", i);
            // initialize network (THIS MUST BE MOVED TO A FUNCTION AND GENERALIZED FOR DIFFERENT NEURON TYPES
            clock_gettime(CLOCK_MONOTONIC, &start);

            #pragma omp parallel for schedule(dynamic, 10) private(j)  // this and the next loops: 4.5 seconds
            for(j = 0; j<snn.n_neurons; j++){
                re_initialize_lif_neuron(&snn, j);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Neurons initialization: %f seconds\n", elapsed_time);

            printf("Neuron reinitialized\n");
            // reinitialize synapses
            clock_gettime(CLOCK_MONOTONIC, &start);

            #pragma omp parallel for schedule(dynamic, 10) private(j) 
            for(j=0; j<snn.n_synapses; j++){
                re_initialize_synapse(&(snn.synapses[j]));
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Synapses initialization: %f seconds\n", elapsed_time);
            printf("Synapses reinitialized\n");

            // introduce spikes into input neurons input synapses (I think that this is not paralelizable)
            // try using pointers directly
            clock_gettime(CLOCK_MONOTONIC, &start);

            for(j=0; j<snn.n_input; j++){ // 3 seconds??
                
                //for(int s = 0; s<snn.lif_neurons[j].n_input_synapse; s++){
                    synapse_t *synap = &(snn.synapses[snn.lif_neurons[j].input_synapse_indexes[0]]);
                    
                    /*
                    synap->l_spike_times = spike_images.spike_images[i].spike_image[j];
                    synap->last_spike += spike_images.spike_images[i].spike_image[j][0];
                    if(synap->last_spike>0)
                        synap->next_spike = 1; // when copying the pointer the first position is the number of spikes
                    synap->max_spikes = synap->last_spike;
                    */
                    ///*
                    for(l = 0; l<spike_images.spike_images[i].spike_image[j][0]; l++){
                        synap->l_spike_times[synap->last_spike] = spike_images.spike_images[i].spike_image[j][l];
                        synap->last_spike +=1;
                    }
                    //synap->max_spikes = synap->last_spike;
                    //*/
                //}
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("SPikes introduction into network: %f seconds\n", elapsed_time);
            
            printf("Spikes introduced on input neurons\n");

            // check that the image has been correctly loaded into the neuron synapse
            /*for(l = 0; l<784; l++){
                if(spike_images.spike_images[i].spike_image[l][0]>0){
                    for(j=0; j<spike_images.spike_images[i].spike_image[l][0]; j++){
                        printf("(%d, %d) ", spike_images.spike_images[i].spike_image[l][j], snn.synapses[snn.lif_neurons[l].input_synapse_indexes[0]].l_spike_times[j]);
                    }
                    printf("\n");
                }
            }*/

            clock_gettime(CLOCK_MONOTONIC, &start);

            for(j=0; j<bins; j++){ // this loop more or less 6 

                clock_gettime(CLOCK_MONOTONIC, &start_bin);

                // process training sample
                //printf("processing bin %d\n", j);
                #pragma omp parallel num_threads(n_process)
                {
                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 10) private(l) 
                    for(l=0; l<snn.n_neurons; l++){
                        input_step(&snn, j, l, generated_spikes);
                    } 
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    input_neurons_elapsed_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Input neurons: %f seconds\n", input_neurons_elapsed_time);

                    //printf("Input synapses processed\n");

                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 10) private(l)
                    for(l=0; l<snn.n_neurons; l++){
                        output_step(&snn, j, l, generated_spikes);
                    }
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    output_neurons_elapsed_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Output neurons: %f seconds\n", output_neurons_elapsed_time);

                    //printf("Output synapses processed\n");
                    // stdp

                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 50) private(l)
                    for(l  = 0; l<snn.n_synapses; l++){
                        snn.synapses[l].learning_rule(&(snn.synapses[l])); 
                    }
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    synapses_elapse_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Synapses: %f seconds\n", synapses_elapse_time);

                    //printf("Training rules processed\n");
                    // store generated output into another variable


                    // train?
                    clock_gettime(CLOCK_MONOTONIC, &end_bin);
                    elapsed_time = (end_bin.tv_sec - start_bin.tv_sec) + (end_bin.tv_nsec - start_bin.tv_nsec) / 1e9;
                    printf("Bin simulation: %f seconds\n", elapsed_time);
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Sample simulation: %f seconds\n", elapsed_time);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &total_end);

    double total_elapsed_time = (total_end.tv_sec - total_start.tv_sec) + (total_end.tv_nsec - total_start.tv_nsec) / 1e9;


    FILE *output_file;
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

    return 0;
}
