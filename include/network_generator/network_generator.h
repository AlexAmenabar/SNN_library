#ifndef NETWORK_GENERATOR_H
#define NETWORK_GENERATOR_H

// struct to store network data (number of neuron, ...) --> this data is written into the network file
typedef struct {
    // general information
    int n_neurons, n_input_neurons, n_output_neurons, n_synapses, n_input_synapses, n_output_synapses;

    // neurons section
    int behaviour, *behaviour_list, r, *r_list, refract_time, *refract_time_list, *n_input_synapses_per_neuron, *n_output_synapses_per_neuron;
    float v_thres, *v_thres_list, v_rest, *v_rest_list;

    // synapses section
    int latency, *latency_list, training_zones, *training_zones_list, **connections;
    float *weights;
    // STDP
    int plus_A, *plus_A_list, minus_A, *minus_A_list; // TODO 
} network_data_t;


// struct to store configuration options to create the network
typedef struct {
    // general
    int neurons, synapses, integers, max_connections_pair_neurons;
    char *output_file_name;
    double lambda, lambda_delays;
    
    int behaviour, v_thres, v_rest, R, refract_time, latency, training_zone, weight; // these variables are used to manage what parameters are provided in configuration file for aleatorization

    // neurons configuration
    int min_behaviour, max_behaviour, min_R, max_R, min_refract_time, max_refract_time;
    float min_v_thres, max_v_thres, min_v_rest, max_v_rest;

    int min_latency, max_latency, min_training_zone, max_training_zone;
    float min_weight, max_weight;


    // the way the output must be generated
    int output_is_separated;
} configuration_t;



void generate_semi_random_synaptic_connections(network_data_t *network_data, configuration_t *conf);
void generate_random_synaptic_connections(network_data_t *network_data, configuration_t *conf);
void generate_random_synaptic_weights(network_data_t *network_data, configuration_t *conf);
void generate_random_synaptic_delays(network_data_t *network_data, configuration_t *conf);//int n_neurons, int **synaptic_connections){
void generate_random_training_type(network_data_t *network_data, configuration_t *conf);

void generate_neurons_data(network_data_t *network_data, configuration_t *conf);

void read_configuration_file(char *file_name, network_data_t *network_data, configuration_t *conf);
void store_network_in_toml_format(char *file_name, network_data_t *network_data, configuration_t *conf);

#endif