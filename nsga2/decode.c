/* Routines to decode the population */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

#include "neuron_models/lif_neuron.h"
#include "training_rules/stdp.h"

/* Function to decode a population to find out the binary variable values based on its bit pattern */
void decode_pop (NSGA2Type *nsga2Params, population *pop)
{
    int i;
    if (nsga2Params->nbin!=0)
    {
        for (i=0; i<nsga2Params->popsize; i++)
        {
            decode_ind (nsga2Params, &(pop->ind[i]));
        }
    }
    return;
}

/* Function to decode an individual to find out the binary variable values based on its bit pattern */
void decode_ind (NSGA2Type *nsga2Params, individual *ind)
{
    /*int j, k;
    int sum;
    if (nsga2Params->nbin!=0)
    {
        for (j=0; j<nsga2Params->nbin; j++)
        {
            sum=0;
            for (k=0; k<nsga2Params->nbits[j]; k++)
            {
                if (ind->gene[j][k]==1)
                {
                    sum += pow(2,nsga2Params->nbits[j]-1-k);
                }
            }
            ind->xbin[j] = nsga2Params->min_binvar[j] + (double)sum*(nsga2Params->max_binvar[j] - nsga2Params->min_binvar[j])/(double)(pow(2,nsga2Params->nbits[j])-1);
        }
    }*/

    int i, j, k, l, s;
    int n_synapses=0;

    // set neuron type
    spiking_nn_t *snn = ind->snn;
    snn->neuron_type = nsga2Params->neuron_type;
    snn->n_neurons = ind->n_neurons;

    // set function pointers
    switch (snn->neuron_type == 0) // this sould be moved to another function, but in the library too TODO
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

    /* INITIALIZE NETWORK NEURONS */
    int *neurons_input_synapses = (int *)calloc(snn->n_neurons, sizeof(int));
    int *neurons_output_synapses = (int *)calloc(snn->n_neurons, sizeof(int));

    // count amount of synapses per each neuron TODO: input synapses??? output synapses???
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    // loop over all synaptic connections to count the number of input and output connections per neuron
    while(synapse_node->value != 0){
        int row = synapse_node->row;
        int col = synapse_node->col;

        // TODO: indicate in some way if the connection is excitatory of inhibitory
        neurons_input_synapses[col] += 1; // THIS DEPENDS ON VALUE, SO CHANGE
        neurons_output_synapses[row] += 1;

        n_synapses += abs(synapse_node->value); // this should be counted when creating the dynamic list
    }

    // initialize neuron parameters // TODO: This should be a function
    neuron_node_t *neuron_node = ind->neurons;
    for(i=0; i<snn->n_neurons; i++){
        if(snn->neuron_type == 0){ // LIF neuron (TODO: this should be a function)
            snn->lif_neurons[i].v_tresh = neuron_node->threshold;
            snn->lif_neurons[i].v_rest = neuron_node->v_rest;
            snn->lif_neurons[i].r_time = neuron_node->refract_time;
            snn->lif_neurons[i].r = neuron_node->r;

            // TODO: neuron is input, neuron is output...

            // set number of output and input synapses
            snn->lif_neurons[i].n_input_synapse = neurons_input_synapses[i];
            snn->lif_neurons[i].n_output_synapse = neurons_output_synapses[i];

                // allocate memory for input and output synapses
            snn->lif_neurons[i].input_synapse_indexes = malloc(neurons_input_synapses[i]);
            snn->lif_neurons[i].output_synapse_indexes = malloc(neurons_output_synapses[i]);

            // some control variables
            snn->lif_neurons[i].next_input_synapse = 0;
            snn->lif_neurons[i].next_output_synapse = 0;

            snn->lif_neurons[i].t_last_spike = -1;

            // move to next neuron node
            neuron_node = neuron_node->next_neuron;
        }
    }



    /* INITIALIE NETWORK SYNAPSES */
    // read synapse parameters from lists
    snn->n_synapses = n_synapses;

    // allocate memory for synapses
    snn->synapses = (synapse_t *)malloc(n_synapses * sizeof(synapse_t));

    // get first synapse
    synapse_node = ind->connectivity_matrix;

    // TODO: revise and move to a function
    for(int i = 0; i<n_synapses; i++)
        //snn->synapses[i].w = lists->weight_list[synapse_id];//w;
        snn->synapses[i].delay = synapse_node->latency;//delay;

        // TODO: THIS MUST BE REVISED, IN GENERAL THE WAY I MANAGE THE INPUT
        /*if(synapse_id < snn->n_input_synapses || synapse_id >= snn->n_synapses - snn->n_output_synapses)//synapse_id > snn->n_output_synapses)
            snn->synapses[i].max_spikes = INPUT_MAX_SPIKES;
        else    
            snn->synapses[i].max_spikes = MAX_SPIKES;
        */

        // initialize control parameters
        snn->synapses[i].t_last_post_spike = -1;
        snn->synapses[i].t_last_pre_spike = -1;
        
        snn->synapses[i].post_neuron_computed = -1;
        snn->synapses[i].pre_neuron_computed = -1;
        
        snn->synapses[i].last_spike = 0; 
        snn->synapses[i].next_spike = 0;

        snn->synapses[i].l_spike_times = (int *)malloc(snn->synapses[i].max_spikes * sizeof(int));
        for(int i = 0; i<snn->synapses[i].max_spikes; i++)
            snn->synapses[i].l_spike_times[i] = -1; // no spikes yet


        // set training rule
        switch (synapse_node->learning_rule) // get synapse training zone from list
        {
        case 0:
            snn->synapses[i].learning_rule = &add_stdp;//(void (*)())&add_stdp;
            break;
        /*case 1:
            synapse->learning_rule = &mult_stdp;//(void (*)())&mult_stdp;
            break;
        case 2:
            synapse->learning_rule = &anti_stdp;//(void (*)())&anti_stdp;
            break;
        //case 3:
        //    synapse->learning_rule = &triplet_stdp;//(void (*)())&triplet_stdp;
        //    break;*/
        default:
            snn->synapses[i].learning_rule = &add_stdp;//(void (*)())&add_stdp;
            break;
        

        // get next synapse
        synapse_node = synapse_node->next_element;
    }


    /* MAKE CONNECTIONS */

    // loop over synapses list to connect the neurons
    synapse_node = ind->connectivity_matrix;
    int next_synapse = 0;
    for(i=0; i<snn->n_synapses; i++){
        int n_synaps = abs(synapse_node->value);

        for(int j = 0; j<n_synaps; j++){
            add_input_synapse_to_neuron(snn, synapse_node->col, next_synapse);
            add_output_synapse_to_neuron(snn, synapse_node->row, next_synapse);
            next_synapse++;
        }
        synapse_node = synapse_node->next_element;
    }

    // free memory
    free(neurons_input_synapses);
    free(neurons_output_synapses);
}