#ifndef SNNFUNCS_H
#define SNNFUNCS_H

#include "nsga2.h"
#include "snn_library.h"

/**
    Functions used to adapt the Pultsito library functions to the data type of this code 
*/


/* Functions related to neurons */

/// @brief
/// @param
void initialize_neurons_from_genotype(spiking_nn_t *snn, individual *ind, int *n_input_synapses, int *n_output_synapses, NSGA2Type *nsga2Params);

/// @brief 
/// @param
void initialize_LIF_neurons_from_genotype(spiking_nn_t *snn, individual *ind, int *n_input_synapses, int *n_output_synapses, NSGA2Type *nsga2Params);

/// @brief
/// @param
void reinitialize_LIF_neurons_from_genotype(spiking_nn_t *snn, individual *ind);


/* Functions related to synaptic connections */

/// @brief
/// @param
void initialize_synapses_from_genotype(NSGA2Type *nsga2Params, spiking_nn_t *snn, individual *ind);

/// @brief
/// @param
void reinitialize_synapses_from_genotype(spiking_nn_t *snn);
void reinitialize_synapse_from_genotype(spiking_nn_t *snn, int index);


/* Neurons and synapses connections */

/// @brief
/// @param
void connect_neurons_by_synapses_from_genotype(spiking_nn_t *snn, individual *ind, NSGA2Type *nsga2Params);

#endif