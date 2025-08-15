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

#ifdef DEBUG
    // print general information about the simulation / training
    printf("===================\n");
    printf("Execution properies:\n");
    printf(" - Execution type = %d\n", execution_type);
    printf(" - Neuron type for exectution = %d\n", neuron_type);
    printf(" - Simulation / training / testing = %d\n", simulation_type);
    printf(" - Number neurons = %d\n", n_neurons);
    printf(" - Number input neurons = %d\n", n_input);
    printf(" - Number output neurons = %d\n", n_output);
    printf(" - Number synapses = %d\n", n_synapses);
    printf(" - Number input synapses = %d\n", n_input_synapses);
    printf(" - Number output synapses = %d\n", n_output_synapses);
    printf(" - Neurons behaviour: ");
    print_array(neuron_excitatory, n_neurons);
    printf(" - Synapses:\n");
    for(i = 0; i<(n_neurons + 1); i++){
        printf("Printing neuron %d connections %d: ", i-1, synaptic_connections[i][0]); // -1 input synapses
        for(j = 0; j<(synaptic_connections[i][0]); j++){
            printf("%d ", synaptic_connections[i][j*2+1]);
            printf("%d ", synaptic_connections[i][j*2+2]);

        }
        printf("\n");
    }
    printf(" - Weights list: ");
    print_array_f(weight_list, n_synapses);
    printf(" - Delays list: ");
    print_array(delay_list, n_synapses);
    printf(" - Training zones: ");
    print_array(training_zones, n_synapses);
    printf("===================\n\n");

#endif


    // initialize snn structure
    spiking_nn_t snn;
    initialize_network(&snn, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses, 
                        neuron_excitatory, synaptic_connections, weight_list, delay_list, training_zones);

#ifdef DEBUG
    printf("Network initialized\n");
#endif

    // free memory
    //for(i=0; i<(n_neurons+1); i++){
    //    free(synaptic_connections[i]);
    //}
    free(synaptic_connections);
    free(weight_list);
    free(delay_list);
    free(training_zones);

#ifdef DEBUG
    // print information of network neurons
    printf("Printing neuron information...\n");
    for(i = 0; i<snn.n_neurons; i++){
        if(snn.neuron_type == 0){
            printf("Neuron %d: \n - V = %f\n - Excitatory = %d\n - Is input = %d\n - Is output = %d\n - n input synapses = %d\n - n output synapses = %d\n", 
                    i, snn.lif_neurons[i].v, snn.lif_neurons[i].excitatory, snn.lif_neurons[i].is_input_neuron, snn.lif_neurons[i].is_output_neuron, 
                    snn.lif_neurons[i].n_input_synapse, snn.lif_neurons[i].n_output_synapse);
        }
    }
    printf("===================\n\n");
#endif

#ifdef DEBUG
    // print information of network synapses
    printf("Printing synapses information...\n");
    for(i = 0; i<snn.n_synapses; i++)
        printf("Synapse %d: \n - W = %f\n - Delay = %d\n", 
                i, snn.synapses[i].w, snn.synapses[i].delay);
    printf("===================\n\n");
#endif

#ifdef DEBUG
    // print information of network synapses
    printf("Printing neuron input synapses...\n");
    for(i = 0; i<snn.n_neurons; i++){
        printf("Neuron %d inpyt synapse indexes (%d): ", i, snn.lif_neurons[i].n_input_synapse);
        for(int j = 0; j<snn.lif_neurons[i].n_input_synapse; j++){
            printf("%d ", snn.lif_neurons[i].input_synapse_indexes[j]);
        }
        printf("\n");
    }
       
    printf("===================\n\n");

    printf("Printing neuron output synapses...\n");
    for(i = 0; i<snn.n_neurons; i++){
        printf("Neuron %d output synapse indexes (%d): ", i, snn.lif_neurons[i].n_output_synapse);
        for(int j = 0; j<snn.lif_neurons[i].n_output_synapse; j++){
            printf("%d ", snn.lif_neurons[i].output_synapse_indexes[j]);
        }
        printf("\n");
    }
       
    printf("===================\n\n");
#endif


    load_input_spike_trains_on_snn(argv[6], &snn);

    // variable to store generated spikes
    unsigned char **generated_spikes = malloc(n_neurons * sizeof(unsigned char *));
    for (int i = 0; i<n_neurons; i++)
        generated_spikes[i] = malloc(time_steps * sizeof(unsigned char));


#ifdef DEBUG

    printf("Printing network input synapses input spike times: \n", i);

    // print spike trains
    for(i = 0; i<snn.n_input; i++){
        printf("Printing synapse %d input spike times: ", i);
        if(snn.neuron_type == 0){ // lif neurons
            int synapse_index = snn.lif_neurons[i].input_synapse_indexes[0]; // now there is only one input synapse on network input neurons
            for(int j = 0; j<=snn.synapses[synapse_index].last_spike; j++){
                printf("%d ", snn.synapses[synapse_index].l_spike_times[j]);
            }
        }
        // else{}
        printf("\n");
    }
#endif


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

#ifdef DEBUG
    printf("Initializing training / simulation\n");
#endif

    struct timespec start, end;
    struct timespec start_neurons, end_neurons;
    struct timespec start_synapses, end_synapses;

    double elapsed_time_neurons= 0;
    double elapsed_time_synapses = 0;

   int n_process = strtoul(argv[10], NULL, 10);


   //int n_neuron_threads = n_neurons / 

    // simulation / training
    if(execution_type == 0){ // clock
        #ifdef CUDA
        double v = process_simulation_lif_neuron(&snn, snn.n_neurons, snn.n_synapses, time_steps);
        #else
        clock_gettime(CLOCK_MONOTONIC, &start);
        while(time_step < time_steps){
            //printf("Time: %d/%d\n", time_step, time_steps);
#ifdef DEBUG
            printf("Processing time step %d\n", time_step);
#endif
            // process all neurons
            clock_gettime(CLOCK_MONOTONIC, &start_neurons);

            #pragma omp parallel num_threads(n_process)
            {
                #pragma omp for schedule(static, 10) private(i) 
                for(int i=0; i<snn.n_neurons; i++){
                    input_step(&snn, time_step, i, generated_spikes);
                } 

                #pragma omp for schedule(static, 10) private(i)
                for(int i=0; i<snn.n_neurons; i++){
                    output_step(&snn, time_step, i, generated_spikes);
                }

                #pragma omp single
                {
                clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                }

#ifdef DEBUG
                printf("Processing stdp on time step %d\n", time_step);
#endif
                // stdp
                clock_gettime(CLOCK_MONOTONIC, &start_synapses);
                #pragma omp for schedule(static, 50) private(i)
                for(int i = 0; i<snn.n_synapses; i++){
                    snn.synapses[i].learning_rule(&(snn.synapses[i])); 
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &end_synapses);

            time_step++;
            
            elapsed_time_neurons += (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
            elapsed_time_synapses += (end_synapses.tv_sec - start_synapses.tv_sec) + (end_synapses.tv_nsec - start_synapses.tv_nsec) / 1e9;


            if(time_step % 1000 == 0)
                printf("Time step: %d/%d\n", time_step, time_steps);

#ifdef DEBUG
            printf(" - Printing synapses weights: \n");
            for(i = 0; i<snn.n_synapses; i++){
                printf(" -- Synapse %d: %f\n", i, snn.synapses[i].w);
            }
            printf("\n=======================================\n\n");
#endif        
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        #endif
    }

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", elapsed_time);
    //else{}
    printf(" - Execution time for neurons: %f seconds\n", elapsed_time_neurons);
    printf(" - Execution time for sybnapses: %f seconds\n", elapsed_time_synapses);

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

    return 0;
}
