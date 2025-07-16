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
        #ifdef DEBUG2
        printf(" > > Decoding individual %d...\n", i);
        fflush(stdout);
        #endif
        
        decode_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to decode an individual to find out the binary variable values based on its bit pattern */
void decode_ind (NSGA2Type *nsga2Params, individual *ind)
{
    int i;
    
    // do some general initialization
    spiking_nn_t *snn = ind->snn; 
    snn->neuron_type = nsga2Params->neuron_type; // set the neuron type
    snn->n_neurons = ind->n_neurons; // set the amount of neurons
    snn->n_input = ind->n_input_neurons; // the amount of input neurons is the same as the input spike trains // TODO: in the future it could be interesting to change this
    snn->n_output = 0; // not output neurons are used
    snn->n_synapses = 0;
    snn->n_input_synapses = 0;
    snn->n_output_synapses = 0; // there are no output synapses


    // variables to store number of input and output synapses for each neuron
    int *neurons_input_synapses = (int *)calloc(snn->n_neurons, sizeof(int));
    int *neurons_output_synapses = (int *)calloc(snn->n_neurons, sizeof(int));

    


    #ifdef DEBUG3
        // print motif types
        new_motif_t *motif_node = ind->motifs_new;
        printf(" > > > Printing motifs types: ");
        while(motif_node){
            printf("%d ", motif_node->motif_type);
            motif_node = motif_node->next_motif;
        }
        printf("\n");

        // print connectivity matrix
        int *result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
        get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

        print_synapses_dynamic_list(ind->connectivity_matrix);

        free(result_matrix);

        fflush(stdout);
    #endif


    // set function pointers
    initialize_network_function_pointers(snn); // this function is defined in pultsito
    

    /* count amount of synapses per each neuron */
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    // count synapses
    int counter = 0;
    while(synapse_node){

        // TODO: indicate in some way if the connection is excitatory of inhibitory
        neurons_input_synapses[synapse_node->col] += abs(synapse_node->value);
        counter+= abs(synapse_node->value);
        
        #ifdef DEBUG3
            printf(" Synapse_node.row = %d (%d / %d)\n", synapse_node->row, counter, ind->n_synapses);
            fflush(stdout);
        #endif
        
        neurons_output_synapses[synapse_node->row] += abs(synapse_node->value);

        snn->n_synapses += abs(synapse_node->value); // this should be counted when creating the dynamic list

        // get next synapse node
        synapse_node = synapse_node->next_element;
    }

    
    // count input synapses
    synapse_node = ind->input_synapses;
    while(synapse_node != NULL){
        i++;
        neurons_input_synapses[synapse_node->col] += abs(synapse_node->value); // add input synapses to input neurons

        snn->n_synapses += abs(synapse_node->value); // this should be counted when creating the dynamic list
        snn->n_input_synapses += abs(synapse_node->value); // this should be counted when creating the dynamic list

        // get next synapse node
        synapse_node = synapse_node->next_element;
    }

    // TODO: This must be fixed and computed in another place, I don't like how network input is managed actually
    snn->n_input = ind->n_input_neurons = snn->n_input_synapses;

    /* initialize network neurons */
    initialize_neurons_from_genotype(snn, ind, neurons_input_synapses, neurons_output_synapses, nsga2Params);

    // initialize network synapses
    initialize_synapses_from_genotype(snn, ind, nsga2Params);

    // connect neurons and synapses
    connect_neurons_by_synapses_from_genotype(snn, ind, nsga2Params);

    // free memory
    free(neurons_input_synapses);
    free(neurons_output_synapses);
}