/* Routines to decode the population */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

#include "neuron_models/lif_neuron.h"
#include "training_rules/stdp.h"
#include "snnfuncs.h"

/* Function to decode a population to find out the binary variable values based on its bit pattern */
void decode_pop (NSGA2Type *nsga2Params, population *pop)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        decode_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to decode an individual to find out the binary variable values based on its bit pattern */
void decode_ind (NSGA2Type *nsga2Params, individual *ind)
{
    int i, j, k, l, s;
    int n_synapses=0;

    // set neuron type
    spiking_nn_t *snn = ind->snn;
    snn->neuron_type = nsga2Params->neuron_type;
    snn->n_neurons = ind->n_neurons;
    snn->n_input = nsga2Params->image_size;
    snn->n_output = 0; // not output neurons

    printf("n_neurons = %d\n", snn->n_neurons);

    // variables to store number of input and output synapses for each neuron
    int *neurons_input_synapses = (int *)calloc(snn->n_neurons, sizeof(int));
    int *neurons_output_synapses = (int *)calloc(snn->n_neurons, sizeof(int));

    // TODO: move this to a function
    // set function pointers
    switch (snn->neuron_type)
    { 
        case 0: // LIF neuron
            snn->complete_step = &lif_neuron_step;
            snn->neuron_initializer = &initialize_lif_neuron;
            snn->neuron_re_initializer = &re_initialize_lif_neuron;
            snn->input_step = &lif_neuron_compute_input_synapses;
            snn->output_step = &lif_neuron_compute_output_synapses;

            // allocate memory for neurons
            snn->lif_neurons = (lif_neuron_t *)malloc(snn->n_neurons * sizeof(lif_neuron_t));

            break;
        default:
            break;
    }

    // count amount of synapses per each neuron TODO: input synapses??? output synapses???
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    // loop over all synaptic connections to count the number of input and output connections per neuron
    while(synapse_node->value != 0){
        int row = synapse_node->row;
        int col = synapse_node->col;

        // TODO: indicate in some way if the connection is excitatory of inhibitory
        neurons_input_synapses[col] += abs(synapse_node->value);
        neurons_output_synapses[row] += abs(synapse_node->value);

        n_synapses += abs(synapse_node->value); // this should be counted when creating the dynamic list

        // get next synapse node
        synapse_node = synapse_node->next_element;
    }

    // store number of synapses
    snn->n_synapses = n_synapses;
    snn->n_synapses += nsga2Params->image_size; // each pixel has its own input synapse
    snn->n_input_synapses = nsga2Params->image_size;
    snn->n_output_synapses = 0;


    // TODO: this is temporal
    // add one input synapse to the first image_size neurons, as those are the input neurons (or to a set of neurons?)
    for(i = 0; i<nsga2Params->image_size; i++){
        int index = i % snn->n_neurons;

        neurons_input_synapses[index] += 1;
    }

    // initialize network neurons
    printf("Initializing neurons from genotype\n");
    initialize_neurons_from_genotype(snn, ind, neurons_input_synapses, neurons_output_synapses, nsga2Params);
    printf("Neurons initialized!\n");

    // initialize network synapses
    printf("Initializing synapses from genotype\n");
    initialize_synapses_from_genotype(snn, ind, nsga2Params);
    printf("Synapses initialized!\n");

    // connect neurons and synapses
    printf("Connecting neurons and synapses\n");
    connect_neurons_by_synapses_from_genotype(snn, ind, nsga2Params);
    printf("Neurons and synapses connected!\n");

    // free memory
    free(neurons_input_synapses);
    free(neurons_output_synapses);
}