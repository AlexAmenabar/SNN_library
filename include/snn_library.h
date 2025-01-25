/* GENERAL INCLUDES */
#ifndef SNN_LIBRARY_H
#define SNN_LIBRARY_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


/* Synapse */
typedef struct {
    float w; // if at start it is positive, excitatory, else inhibitory
    int delay; // the delay time of the synapse

    int *l_spike_times; // FIFO list
    int next_spike, last_spike, max_spikes; // indexes to control list
} synapse_t;


/* Artifical Spiking Neuron models */
// LIF
typedef struct {
    synapse_t *input_synapses; // lists of input and output synapses
    int input_synapse_amount;
    synapse_t *output_synapses;
    int output_synapse_amount;

    int actual_input_synapse; // used at initialization
    int actual_output_synapse; // used at initialization

    int excitatory; // 0 if inhibitory, else 1

    float v; // if at start it's postive, excitatory neuron, else inhibitory
    float r;// = 1.0; // membrane resistance
    float v_rest;// = 0;
    float v_reset;// = 0;
    float v_tresh;// = 1;
} lif_neuron_t;

typedef struct {
    synapse_t *input_synapses; // lists of input and output synapses
    int input_synapse_amount;
    synapse_t *output_synapses;
    int output_synapse_amount;

    int actual_input_synapse; // used at initialization
    int actual_output_synapse; // used at initialization

    int excitatory; // 0 if inhibitory, else 1
} spiking_neuron_t;

void initialize_lif_neuron(lif_neuron_t *neuron, int *synapse_matrix, int excitatory_neuron, int length, 
                            int neuron_is_input, int neuron_is_output, int neuron_index);
void step_lif_neuron(spiking_neuron_t *neuron, int t, int neuron_identifier, unsigned char **generated_spikes);


// Hodwking-Huxley model
typedef struct {
    int a;
} hh_neuron_t;

void initialize_hh_neuron();
void step_hh_neuron(hh_neuron_t *neuron, int t, int neuron_identifier, unsigned char **generated_spikes);



/* Motifs */
typedef struct {
    int type; 
    void *neurons; // each motif can have a different amount of neurons depending on the type 
} motif_t;



/**
 * Motifs
 */

typedef struct {

} motif_FFE_t;

typedef struct {

} motif_FBE_t;

typedef struct {

} motif_FBI_t;

typedef struct {

} motif_RCE_t;

typedef struct {

} motif_RCI_t;

typedef struct {

} motif_LTI_t;

typedef struct {

} motif_FFI_t;

typedef struct {

} motif_FFRE_t;

typedef struct {

} motif_FFRI_t;

typedef struct {

} motif_FBRE_t;

typedef struct {

} motif_FBRI_t;

typedef struct {

} motif_FFLI_t;

typedef struct {

} motif_FBLI_t;


/**
 * Spiking Neural Network 
 */
typedef struct {
    int type; // 0 individual neurons, 1 motifs
    lif_neuron_t *lif_neurons; // pointer to neurons 
    hh_neuron_t *hh_neurons;
    int n_neurons;

    //struct 
    int neuron_type; // neuron type identifier

    // hyperparameters
} spiking_nn_t;


#endif
