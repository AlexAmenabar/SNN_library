#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include <stdio.h>
#include "snn_library.h"

// can be changed as input parameters 
#ifndef INPUT_SYNAPSES
#define INPUT_SYNAPSES 0
#endif

#ifndef INPUT_NEURON_BEHAVIOUR
#define INPUT_NEURON_BEHAVIOUR 1
#endif

#ifndef INPUT_WEIGHTS
#define INPUT_WEIGHTS 0
#endif

#ifndef INPUT_DELAYS
#define INPUT_DELAYS 0
#endif

#ifndef INPUT_TRAINING_ZONES
#define INPUT_TRAINING_ZONES 0
#endif


// configuration parameters NOT USED
#ifndef EXECUTION_TYPE // clock or event driven
#define EXECUTION_TYPE 0
#endif

#ifndef NEURON_TYPE // LIF, HH, ... NOT USED
#define NEURON_TYPE 0
#endif

#ifndef EXECUTION_OBJ // simulation or ml
#define EXECUTION_OBJ 0
#endif

#ifndef N_PROCESS // simulation or ml
#define N_PROCESS 1
#endif



/**
* Functions to load data from files (input spike trains, network information...)
*/


/// @brief Function to open a file
/// @param f FILE to stream file 
/// @param file_name File name
/// @return Code
int open_file(FILE **f, const char *file_name);

/// @brief Function to open a file and write on it without overwriting current contents
/// @param f File to write in
/// @param file_name Name of the file to be opened
/// @return Code
int open_file_w(FILE **f, const char *file_name);


/// @brief Function to close a file
/// @param f File to close
void close_file(FILE **f);

/// @brief Load network data from file (number of neurons, number of synapses, connections, weights of synapses...)
/// @param file_name File to load information from
/// @param n_neurons Variable to store number of neurons 
/// @param n_input Number of neuron on the input layer
/// @param n_output Number of neurons on the output layer
/// @param n_synapses Number of synapses on the network
/// @param n_input_synapses Number of input synapses
/// @param n_output_synapses Number of output synapses
/// @param synapse_matrix Matrix to store synaptic connections between neurons
/// @param neuron_excitatory List to store which neurons are excitatory (1) and inhibitory (0)
/// @param weight_list List of synaptic weights
/// @param delay_list List of synaptic delays
/// @param training_zones List of training type for each synapse
//void load_network_information(const char *file_name, int *n_neurons, int *n_input, int *n_output, int *n_synapses, int *n_input_synapses, int *n_output_synapses, 
//                int ***synaptic_connections, int **neuron_excitatory, float **weight_list, int **delay_list, int **training_zones);
void load_network_information(const char *file_name, spiking_nn_t *snn, network_construction_lists_t *lists, simulation_configuration_t *conf);


/// @brief Load input spike trains
/// @param file_name File to load spike trains from
/// @param snn Spiking Neural Network structure to store input spike trains on input synapses
void load_input_spike_trains_on_snn(const char *file_name, spiking_nn_t *snn);


/*/// @brief Insert new content on file init
/// @param file_name Original name
/// @param temp_file_name Temporal file name
void insert_at_file_init(const char *file_name, const char *temp_file_name);*/



/// @brief Read configuration parameters from configuration file
/// @param file_name Configuration file name
/// @param conf struct to store configuration parameters
int load_configuration_params(const char *file_name, simulation_configuration_t *conf);
int load_configuration_params_from_toml(const char *file_name, simulation_configuration_t *conf);


void store_results(simulation_results_t *results, simulation_configuration_t *conf, spiking_nn_t *snn);


#endif