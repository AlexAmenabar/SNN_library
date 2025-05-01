#ifndef LIF_NEURON_H
#define LIF_NEURON_H

#include "snn_library.h"

#define REFRAC_TIME 5
#define T 100 // tiempo total de la simulación (ms) # this should be constants defined on snn_library.h
#define DT 1.0 // paso del tiempo (ms) # this too
#define TC 0.9 // time constant parameter # this too


/**
 * LIF neuron model functions
 */


 /* Functions related to simulation */

 /// @brief Step to process the input synapses of a LIF neuron and compute the membrane potential of the new time step
 /// @param snn Spiking neural network structure
 /// @param t Actual time step
 /// @param neuron_id Identifier of the neuron to be processed
 /// @param results Structure to store results of the simulation
void lif_neuron_compute_input_synapses(spiking_nn_t *snn, int t, int neuron_id, simulation_results_t *results);

/// @brief Step to process the output synapses of a LIF neuron and generate a spike if it is necessary
/// @param snn Spiking neural network structure
/// @param t Actual time step
/// @param neuron_id Identifier of the neuron to be processed
/// @param results Structure to store results of the simulation
void lif_neuron_compute_output_synapses(spiking_nn_t *snn, int t, int neuron_id, simulation_results_t *results);

/// @brief Computes a time step for a lif neuron (process input synapse spikes and generate an spike if necessary)
/// @param snn SNN structure
/// @param t Time step
/// @param neuron_id Identifier of the neuron to be computed
/// @param results Structure to store the results of the simulation
void lif_neuron_step(spiking_nn_t *snn, int t, int neuron_id, simulation_results_t *results);



/* Functions to initialize LIF neurons */

/// @brief Initialize a lif neuron
/// @param snn SNN structure with neuron information
/// @param neuron_index Index to get the neuron to be initialized from snn
/// @param excitatory If the neuron is excitatory or not
/// @param synapse_matrix Synapse matrix to count the number of input and output synapses and reserve memory
void initialize_lif_neuron(spiking_nn_t *snn, int neuron_index, network_construction_lists_t *lists, int n_input_synapse, int n_output_synapse);

/// @brief Initialize a lif neuron
/// @param snn SNN structure with neuron information
/// @param neuron_index Index to get the neuron to be initialized from snn
void re_initialize_lif_neuron(spiking_nn_t *snn, int neuron_index, network_construction_lists_t *lists);



/* Functions to add input and output synapses to LIF neurons */

/// @brief Add an input synapse to the neuron
/// @param neuron Neuron to add the synapse
/// @param synapse Synapse to be added
/// @param synapse_index Synapse to be added to the neuron
void add_input_synapse_to_lif_neuron(lif_neuron_t *neuron, synapse_t* synapse, int synapse_index);

/// @brief Add an output synapse to the neuron
/// @param neuron Neuron to add the synapse
/// @param synapse synapse to be added
/// @param synapse_index Synapse to be added to the neuron
void add_output_synapse_to_lif_neuron(lif_neuron_t *neuron, synapse_t *synapse, int synapse_index);

#endif
