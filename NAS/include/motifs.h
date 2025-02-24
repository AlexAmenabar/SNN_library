#ifndef MOTIFS_H
#define MOTIFS_H

#include "snn_library.h"

typedef struct{
    lif_neuron_t *lif_neurons;
    synapse_t *synaptic_connections;
    int motif_type;
    int input_neurons, output_neurons;
} neuron_motif_t;

void initialize_FFE_motif(neuron_motif_t motif){
    if(type==0){ //FFE_motif

    }
}


void initialize_motif(neuron_motif_t motif, int type, int input_synapses, int output_synapses){
    if(type==0){ //FFE_motif
        initialize_FFE_motif();
    }
}


#endif MOTIFS_H