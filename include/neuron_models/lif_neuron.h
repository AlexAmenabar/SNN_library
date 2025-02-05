#ifndef LIF_NEURON_H
#define LIF_NEURON_H

#include "snn_library.h"

#define REFRAC_TIME
#define T 100 // tiempo total de la simulación (ms)
#define DT 1.0 // paso del tiempo (ms)
#define TC 0.9 // time constant parameter


/**
 * LIF neuron model functions
 */

/// @brief Computes a time step for a lif neuron (process input synapse spikes and generate an spike if necessary)
/// @param snn SNN structure
/// @param t Time step
/// @param neuron_id Identifier to neuron to be computed
/// @param generated_spikes List to store generated spikes
void step_lif_neuron(spiking_nn_t *snn, int t, int neuron_id, unsigned char **generated_spikes);


/// @brief Initialize a lif neuron
/// @param snn SNN structure with neuron information
/// @param neuron_index Index to get the neuron to be initialized from snn
/// @param excitatory If the neuron is excitatory or not
/// @param synapse_matrix Synapse matrix to count the number of input and output synapses and reserve memory
void initialize_lif_neuron(spiking_nn_t *snn, int neuron_index, int excitatory, int **synaptic_connections, int n_input_synapse, int n_output_synapse);


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