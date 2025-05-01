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
/// @return Execution code
int open_file(FILE **f, const char *file_name);


/// @brief Function to open a file and write on it without overwriting current contents
/// @param f File to write in
/// @param file_name Name of the file to be opened
/// @return Execution code
int open_file_w(FILE **f, const char *file_name);


/// @brief Function to close a file
/// @param f File to close
void close_file(FILE **f);


/// @brief Load network data from file (number of neurons, number of synapses, connections, weights of synapses...)
/// @param file_name File to load information from
/// @param snn Spiking nueral network structure to load information in
/// @param lists Lists used to create the SNN
/// @param conf Structure that contains the simulation configuration
void load_network_information(const char *file_name, spiking_nn_t *snn, network_construction_lists_t *lists, simulation_configuration_t *conf);


/// @brief Load input spike trains inside the SNN structure
/// @param file_name File to load spike trains from
/// @param snn Spiking Neural Network structure to store input spike trains on input synapses
void load_input_spike_trains_on_snn(const char *file_name, spiking_nn_t *snn);


/// @brief Read configuration parameters from configuration file
/// @param file_name Configuration file name
/// @param conf struct to store configuration parameters
int load_configuration_params(const char *file_name, simulation_configuration_t *conf);


/// @brief Read configuration parameters from configuration TOML file
/// @param file_name Name of the file to load configuration information from
/// @param conf struct to store configuration parameters
int load_configuration_params_from_toml(const char *file_name, simulation_configuration_t *conf);


/// @brief Function that stores the obtained results in files
/// @param results Structure that contains the results of the simulation
/// @param conf Structure to read where to store the results
/// @param snn Spiking neural network structure
void store_results(simulation_results_t *results, simulation_configuration_t *conf, spiking_nn_t *snn);

#endif