/* GENERAL INCLUDES */
#ifndef SNN_LIBRARY_H
#define SNN_LIBRARY_H

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

#ifndef MAX_SPIKES
#define MAX_SPIKES 1000
#endif


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

    void *learning_rule; // pointer to learning rule function 

    int t_last_pre_spike, t_last_post_spike; // time steps of the last spikes generated by synapse pre synaptic neuron and post synaptic neuron
    int pre_neuron_computed, post_neuron_computed; // used to know when to compute STDP
} synapse_t;


/* Artifical Spiking Neuron models */
// LIF
typedef struct {
    //synapse_t **input_synapses; // lists of input and output synapses
    int *input_synapse_indexes; // indexes to get neuron input synapses from snn.synapses
    int n_input_synapse;
    int next_input_synapse;

    //synapse_t **output_synapses;
    int *output_synapse_indexes; // indexes to get neuron output synapses from snn.synapses
    int n_output_synapse;
    int next_output_synapse;

    // is input ot output neuron
    int is_input_neuron;
    int is_output_neuron;

    int excitatory; // 0 if inhibitory, else 1

    float v; // if at start it's postive, excitatory neuron, else inhibitory
    float r;// = 1.0; // membrane resistance
    float v_rest;// = 0;
    float v_reset;// = 0;
    float v_tresh;// = 1;
} lif_neuron_t;


//void initialize_lif_neuron(lif_neuron_t *neuron, int *synapse_matrix, int excitatory_neuron, int length, 
//                            int neuron_is_input, int neuron_is_output, int neuron_index);
//void step_lif_neuron(spiking_nn_t *snn, int t, int neuron_identifier, unsigned char **generated_spikes);


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
 * Spiking Neural Network 
 */
typedef struct {
    // network general information
    int n_neurons; // n neurons on the network
    int n_input; // number of input neurons
    int n_output; // number of output neurons
    int neuron_type; // type of the neurons of this network
    int n_synapses; // number of synapses of the network

    // pointers to different neuron types
    lif_neuron_t *lif_neurons; 
    hh_neuron_t *hh_neurons;

    // pointer to network synapses
    synapse_t *synapses;

    // hyperparameters
} spiking_nn_t;



#endif
