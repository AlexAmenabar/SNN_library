#ifndef STDP_H
#define STDP_H

#include "../snn_library.h"

void add_stdp(synapse_t *synapse);

void mult_stdp(synapse_t *synapse);

void anti_stdp(synapse_t *synapse);

void triplet_stdp(synapse_t *synapse);

#endif