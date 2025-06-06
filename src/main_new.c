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
    int i, j;
    spiking_nn_t snn;
    simulation_configuration_t conf;
    simulation_results_t results;
    network_construction_lists_t lists;

    // randomize execution
    srand(time(NULL));

    // load configuration parameters from input file
    //load_configuration_params(argv[1], &conf);
    printf("Loading configuration parameters...\n");
    load_configuration_params_from_toml(argv[1], &conf);
    printf("Configuration parameters loaded!\n\n");

    // load information about the snn from the network file
    printf("Loading network...\n");
    load_network_information(conf.network_file, &snn, &lists, &conf);
    printf("Network information loaded!\n\n");


    // initialize the network
    printf("Initializing network...\n"); 
    initialize_network(&snn, &conf, &lists);
    printf("Network initialized!\n\n");


    // load input spike train from file (different depending on execution type) // ESTO DEBERÍA CAMBIARLO; NO ME TERMINA DE GUSTAR COMO ESTÁ PASANDO DIRECTAMENTE EL PARÁMETRO DE ENTRADA
    printf("Loading input spike trains...\n");
    load_input_spike_trains_on_snn(conf.input_spikes_file, &snn);
    printf("Spike trains loaded!\n");

    // initialize struct to store results
    printf("Initializing results struct...\n");
    initialize_results_struct(&results, &conf, &snn);
    printf("Results strcut initialized!\n");


#ifdef REORDER
    printf("Reordering synapses list...\n");
    reorder_synapse_list(&snn);
    printf("List of synapses reordered!\n");
#endif

/*
    // reorder list of synaptic connections
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
    
// print general information about the simulation: simulation configuration, network information...
#ifdef DEBUG //////////////////////////////////////////////////////////////////////////////////////////////
/*
    printf("===================\n");
    printf("Execution properies:\n");
    printf(" - Execution type = %d\n", conf.simulation_type);
    printf(" - Neuron type for exectution = %d\n", snn.neuron_type);
    //printf(" - Simulation / training / testing = %d\n", simulation_type);
    printf(" - Number neurons = %d\n", snn.n_neurons);
    printf(" - Number input neurons = %d\n", snn.n_input);
    printf(" - Number output neurons = %d\n", snn.n_output);
    printf(" - Number synapses = %d\n", snn.n_synapses);
    printf(" - Number input synapses = %d\n", snn.n_input_synapses);
    printf(" - Number output synapses = %d\n", snn.n_output_synapses);
    printf(" - Neurons behaviour: ");
    print_array(lists.neuron_excitatory, snn.n_neurons);
    printf(" - Synapses:\n");
    for(i = 0; i<(snn.n_neurons + 1); i++){
        printf("Printing neuron %d connections %d: ", i-1, lists.synaptic_connections[i][0]); // -1 input synapses
        for(j = 0; j<(lists.synaptic_connections[i][0]); j++){
            printf("%d ", lists.synaptic_connections[i][j*2+1]);
            printf("%d ", lists.synaptic_connections[i][j*2+2]);

        }
        printf("\n");
    }
    printf(" - Weights list: ");
    print_array_f(lists.weight_list, snn.n_synapses);
    printf(" - Delays list: ");
    print_array(lists.delay_list, snn.n_synapses);
    printf(" - Training zones: ");
    print_array(lists.training_zones, snn.n_synapses);
    printf("===================\n\n");

    ////////////////////////////////////////////////////////////////////////////////////////////

    /*
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
*/
    //////////////////////////////////////////////////////////////////////////////////////////////
/*
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
    */
//#endif

    // free memory TODO: must be corrected
    //free_lists_memory(&lists, &snn);

    printf("Initializing training / simulation\n");

    // Run the simulation

#ifndef BY_SAMPLE
    simulate(&snn, &conf, &results);
#else
    simulate_by_samples();
#endif


    // store simulation results
    store_results(&results, &conf, &snn);

    // free memory

    // print execution times
    printf("Execution time: %f seconds\n", results.elapsed_time);
    printf(" - Execution time for neurons: %f seconds\n", results.elapsed_time_neurons);
    printf("    - Execution time for neurons input: %f seconds\n", results.elapsed_time_neurons_input);
    printf("    - Execution time for neurons output: %f seconds\n", results.elapsed_time_neurons_output);
    printf(" - Execution time for sybnapses: %f seconds\n", results.elapsed_time_synapses);

    return 0;
}
