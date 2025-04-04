#include "snn_library.h"
#include "load_data.h"
#include "helpers.h"
#include "training_rules/stdp.h"

#include "neuron_models/lif_neuron.h"
//#include "neuron_models/GPU_lif_neuron.cuh"

#include "simulations/simulations.h"


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* main.c */
int main(int argc, char *argv[]) {
    // variables definition

    // SOME INT SHOULD BE INT8 OR AT LEAST UNSIGNED
    int execution_type, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses;
    int simulation_type, time_steps, learn, n_process;

    int **synaptic_connections, *neuron_excitatory, *training_zones; // change to uint in the future
    int *delay_list;
    float *weight_list;

    void *neuron_initializer; // function to initialize neurons
    void (*input_step)(), (*output_step)(); // functions to run a step on the simulation

    int i, j;

    // network structure
    spiking_nn_t snn;
    
    // randomize execution
    srand(time(NULL));
    
    
    // Load information from input parameters
    execution_type = strtoul(argv[1], NULL, 10);
    neuron_type = strtoul(argv[2], NULL, 10);
    simulation_type = strtoul(argv[3], NULL, 10);

    // load information about the snn from file
    load_network_information(argv[4], &n_neurons, &n_input, &n_output, &n_synapses, &n_input_synapses, &n_output_synapses,
                            &synaptic_connections, &neuron_excitatory, &weight_list, &delay_list, &training_zones);


    time_steps = strtoul(argv[5], NULL, 10);



// print general information about the simulation: simulation configuration, network information...
#ifdef DEBUG //////////////////////////////////////////////////////////////////////////////////////////////
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
#endif ///////////////////////////////////////////////////////////////////////////////////////////////////////////



    // initialize the network 
    initialize_network(&snn, neuron_type, n_neurons, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses, 
                        neuron_excitatory, synaptic_connections, weight_list, delay_list, training_zones);



#ifdef DEBUG /////////////////////////////
    printf("Network initialized\n");


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


    // print information of network synapses
    printf("Printing synapses information...\n");
    for(i = 0; i<snn.n_synapses; i++)
        printf("Synapse %d: \n - W = %f\n - Delay = %d\n", 
                i, snn.synapses[i].w, snn.synapses[i].delay);
    printf("===================\n\n");


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
#endif ///////////////////////////////////



    // ESTO HAY QUE MIRARLO
    // free memory
    //for(i=0; i<(n_neurons+1); i++){
    //    free(synaptic_connections[i]);
    //}
    free(synaptic_connections);
    free(weight_list);
    free(delay_list);
    free(training_zones);


    // load input spike train from file // ESTO DEBERÍA CAMBIARLO; NO ME TERMINA DE GUSTAR COMO ESTÁ PASANDO DIRECTAMENTE EL PARÁMETRO DE ENTRADA
    load_input_spike_trains_on_snn(argv[6], &snn);


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

    // ESTO HAY QUE MOVERLO DE SITIO; Y HAY QUE CAMBIAR LA MANERA DE LEER LOS PARÁMETROS DE ENTRADA, ESTO ES UN DESASTRE, IGUAL POR UN FICHERO
    n_process = strtoul(argv[10], NULL, 10);

    printf("Initializing training / simulation\n");


    // initialize struct to store simulation configuration and outputs
    simulation_configuration_t conf;
    conf.simulation_type = execution_type;
    conf.neuron_type = neuron_type;
    conf.learn = learn;
    conf.n_process = n_process;
    conf.time_steps = time_steps;

    simulation_results_t results;
    results.elapsed_time = 0;
    results.elapsed_time_neurons = 0;
    results.elapsed_time_synapses = 0;

    // variable to store generated spikes // ESTO IGUAL MEJOR QUE SEA UN STRUCT, NO ME GUSTA, PERO POR AHORA OK
    // CAMBIARLO POR TIEMPOS DE SPIKES, LAS LISTAS SERÁN MÁS CORTAS
    results.generated_spikes = malloc(n_neurons * sizeof(unsigned char *));
    for (i = 0; i<n_neurons; i++)
        results.generated_spikes[i] = malloc(time_steps * sizeof(unsigned char));



    // call simulation function
    // if simulation{} else if sample_simulation{}
    simulate(&snn, input_step, output_step, &conf, &results);


    // IFDEFS TO INDICATE WHAT MUST BE STORED
    // STORE AMOUNT OF SPIKES FOR EACH NEURON TOO

    // LO DE ABAJO ORGANIZARLO EN FUNCIONES, ESTO ES UN DESASTRE
    printf("Execution time: %f seconds\n", results.elapsed_time);
    //else{}
    printf(" - Execution time for neurons: %f seconds\n", results.elapsed_time_neurons);
    printf(" - Execution time for sybnapses: %f seconds\n", results.elapsed_time_synapses);
/*
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
        for(int j = 0; j<conf.time_steps; j++){
            fprintf(output_file, "%c", results.generated_spikes[i][j]);
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
    fprintf(output_file3, "%f ", resuts.elapsed_time);
    fprintf(output_file3, "%f ", results.elapsed_time_neurons);
    fprintf(output_file3, "%f \n", results.elapsed_time_synapses);

    fclose(output_file);
    fclose(output_file2);
    fclose(output_file3);*/
    // free memory
//    free(snn.lif_neurons);
//    free(snn.synapses);

    return 0;
}
