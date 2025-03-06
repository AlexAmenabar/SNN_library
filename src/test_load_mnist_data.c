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
    float *weight_list;

    void *neuron_initializer; // function to initialize neurons
    void (*input_step)(), (*output_step)(); // functions to run a step on the simulation

    int i, j;

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
    spike_images.spike_images = malloc(60000 * sizeof(spike_image_t)); // alloc memory for images
    
    int pT = 100;
    int pDT = 1;
    int bins = (int)(pT/pDT);

    spike_images.bins = bins;
    spike_images.image_size = 784;
    spike_images.n_images = 60000;

    printf("Converting images to spike trains...\n");

    for(int i=0; i<spike_images.n_images; i++){
        // alloc memory for image pixels
        spike_images.spike_images[i].spike_image = malloc(spike_images.image_size * sizeof(int *)); // each image has 728 pixels (this must be generalized)

        for(int j=0; j<spike_images.image_size; j++){ // alloc bins integers for each pixel spike train --> this must be corrected, as bins is used for 0,1 spike trains, but I use spike times, so less positions are needed
            spike_images.spike_images[i].spike_image[j] = malloc(bins * sizeof(int));
        }
    }

    printf("Memory reserved!\n");

    // convert images into spike images
    convert_images_to_spikes_by_poisson_distribution(&spike_images, train_image, spike_images.n_images, spike_images.image_size, spike_images.bins);

    //int *training_set_spikes = (int *)malloc(6000 * sizeof(int));
    //int *test_set_spikes = (int * )malloc(1000 * sizeof(int));
    // convert mnist information into spikes

    printf("Conversion correctly completed!\n");

    // print spike trains
/*
    printf("Printing spike trains...\n");
    for(int i = 0; i<spike_images.n_images; i++){
        printf("Image %d\n", i);
        for(int j = 0; j<spike_images.image_size; j++){
            printf("Pixel %d, value %f, Spike train: ", j, train_image[i][j]);
            for(int l=0; l<spike_images.spike_images[i].spike_image[j][0]+1; l++){
                printf("%d ", spike_images.spike_images[i].spike_image[j][l]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
*/


    // variable to store generated spikes
    unsigned char **generated_spikes = malloc(n_neurons * sizeof(unsigned char *));
    for (int i = 0; i<n_neurons; i++)
        generated_spikes[i] = malloc(time_steps * sizeof(unsigned char));


    // Initialize step function depending on neuron type
    switch (neuron_type)
    {
        case 0: // LIF
            //step = &step_lif_neuron;
            input_step = &lif_neuron_compute_input_synapses;
	        output_step = &lif_neuron_compute_output_synapses;
	    break;
        case 1: // HH
            break;
        default:
            break;
    }

    // load amount of threads 
    int n_process = strtoul(argv[10], NULL, 10);

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
        for(int i = 0; i<5000; i++){
            // initialize network
            

            // introduce spikes into neurons spikes


            // process training sample


            // store generated output into another variable


            // train?
        }
    }

    // print spikes generated by each neuron
    FILE *output_file;
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
    FILE *output_file2;
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
    fprintf(output_file3, "%f ", elapsed_time);
    fprintf(output_file3, "%f ", elapsed_time_neurons);
    fprintf(output_file3, "%f \n", elapsed_time_synapses);

    fclose(output_file);
    fclose(output_file2);
    fclose(output_file3);
    // free memory
//    free(snn.lif_neurons);
//    free(snn.synapses);
*/
    return 0;
}
