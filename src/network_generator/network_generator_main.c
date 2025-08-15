#include "network_generator/network_generator.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    network_data_t network_data;
    configuration_t conf;
    
    // randomize exection to create different networks each time
    srand(time(NULL));

    printf("Reading configuration file...\n");
    read_configuration_file(argv[1], &network_data, &conf);
    printf("Configuration file readed! Printing general information: \n");

    printf(" - Neurons provided = %d\n", conf.neurons);
    printf(" - Synapses provided = %d\n", conf.synapses);

    printf(" - Neurons = %d\n", network_data.n_neurons);
    printf(" - Input neurons = %d\n", network_data.n_input_neurons);
    printf(" - Output neurons = %d\n", network_data.n_output_neurons);
    printf(" - Synapses = %d\n", network_data.n_synapses);
    printf(" - Input synapses = %d\n", network_data.n_input_synapses);
    printf(" - Output synapses = %d\n", network_data.n_output_synapses);

    // reserve memory for arrays related to neurons
    network_data.connections = (int **)malloc((network_data.n_neurons+1) * sizeof(int *)); // n + 1 as there are n neurons and 1 network input "layer"
    network_data.behaviour_list = (int *)malloc(network_data.n_neurons * sizeof(int));
    network_data.r_list = (int *)malloc(network_data.n_neurons * sizeof(int));
    network_data.refract_time_list = (int *)malloc(network_data.n_neurons * sizeof(int));
    network_data.v_thres_list = (double *)malloc(network_data.n_neurons * sizeof(double));
    network_data.v_rest_list = (double *)malloc(network_data.n_neurons * sizeof(double));
    network_data.n_input_synapses_per_neuron = (int *)malloc(network_data.n_neurons * sizeof(int));
    network_data.n_output_synapses_per_neuron = (int *)malloc(network_data.n_neurons * sizeof(int));


    // CHOOSE ONE DEPENDING ON COMPILE PARAMETER
    //generate_semi_random_synaptic_connections(synaptic_connections, n, n_input, n_output, &n_synapses, &n_input_synapses, &n_output_synapses);

    // THIS IS RELTATED TO THE IF STATEMENT MENTIONED BEFORE
    network_data.n_synapses = 0;
    network_data.n_input_synapses = 0;
    network_data.n_output_synapses = 0;

    printf("Generating synaptic connections...\n");
    generate_random_synaptic_connections(&network_data, &conf);
    printf("Synaptic connections generated!\n\n");
    

    // alloc memory for arrays related to synaptic connections
    network_data.latency_list = (int *)malloc(network_data.n_synapses * sizeof(int));
    network_data.weights = (double *)malloc(network_data.n_synapses * sizeof(double));
    network_data.training_zones_list = (int *)malloc(network_data.n_synapses * sizeof(int));


    // generate random behaviours
    printf("Generating neurons data...\n");
    generate_neurons_data(&network_data, &conf);
    printf("Neurons data generated!\n\n");

    printf("Generating weights...\n");
    generate_random_synaptic_weights(&network_data, &conf);
    printf("Weights generated!\n\n");

    printf("Generating delays...\n");
    generate_random_synaptic_delays(&network_data, &conf);
    printf("Delays generated!\n\n");

    printf("Generating learning rules...\n");
    generate_random_training_type(&network_data, &conf);
    printf("Learning rules generated!\n\n");


    printf("Network completely generated! Storing network...\n");
    store_network_in_toml_format(conf.output_file_name, &network_data, &conf);
    printf("Network succesfully stored!\n");
}