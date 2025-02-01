#ifndef STDP_H
#define STDP_H

#include "snn_library.h"

/// @brief Additive STDP computation
/// @param synapse Synapse to be updated
void add_stdp(synapse_t *synapse);


/// @brief Multiplicative STDP computation
/// @param synapse Synapse to be updated
void mult_stdp(synapse_t *synapse);


/// @brief Anti STDP computation
/// @param synapse Synapse to be updated
void anti_stdp(synapse_t *synapse);


/// @brief Tiplet STDP computation
/// @param synapse Synapse to be updated
void triplet_stdp(synapse_t *synapse);

#endif