#include "snnfuncs.h"
#include "training_rules/stdp.h"

/* Functions related to neurons */

void initialize_LIF_neurons_from_genotype(spiking_nn_t *snn, individual *ind, int *n_input_synapses, int *n_output_synapses, NSGA2Type *nsga2Params){
    int i;
    lif_neuron_t *lif_neuron;
    neuron_node_t *neuron_node = ind->neurons;
    
    for(i=0; i<snn->n_neurons; i++){
        //printf("Initializing neuron %d\n", i);

        // get the neuron from the snn neuron list
        lif_neuron = &(snn->lif_neurons[i]);

        // copy information from the genotype to the lif neuron
        lif_neuron->v_tresh = neuron_node->threshold;
        lif_neuron->v_rest = neuron_node->v_rest;
        lif_neuron->r_time = neuron_node->refract_time;
        lif_neuron->r = neuron_node->r;

        // TODO: neuron is input, neuron is output...
        if(i < snn->n_input){
            lif_neuron->is_input_neuron = 1;
        }
        else{
            lif_neuron->is_input_neuron = 0;
        }

        // set number of output and input synapses
        lif_neuron->n_input_synapse = n_input_synapses[i];
        lif_neuron->n_output_synapse = n_output_synapses[i];

        // allocate memory for input and output synapses
        lif_neuron->input_synapse_indexes = malloc(n_input_synapses[i]);
        lif_neuron->output_synapse_indexes = malloc(n_output_synapses[i]);

        // set initial values for some control variables
        lif_neuron->next_input_synapse = 0;
        lif_neuron->next_output_synapse = 0;
        lif_neuron->t_last_spike = -1;

        // move to next neuron node
        neuron_node = neuron_node->next_neuron;
    }
}

void reinitialize_LIF_neurons_from_genotype(spiking_nn_t *snn, individual *ind){

}

void initialize_neurons_from_genotype(spiking_nn_t *snn, individual *ind, int *n_input_synapses, int *n_output_synapses, NSGA2Type *nsga2Params){
    switch(snn->neuron_type){
        case 0: //lif neuron
            initialize_LIF_neurons_from_genotype(snn, ind, n_input_synapses, n_output_synapses, nsga2Params);
            break;

        // case 1...
        default:
            initialize_LIF_neurons_from_genotype(snn, ind, n_input_synapses, n_output_synapses, nsga2Params);
            break;
    }
}


/* Functions related to synaptic connections */

void initialize_synapses_from_genotype(spiking_nn_t *snn, individual *ind, NSGA2Type *nsga2Params){
    int i,j,k;
    synapse_t *synapse;
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    // allocate memory for synaptic connections
    snn->synapses = (synapse_t *)malloc(snn->n_synapses * sizeof(synapse_t));

    printf("Total number of synapses = %d, input synapses = %d, middle synapses %d\n", snn->n_synapses, nsga2Params->image_size, snn->n_synapses - nsga2Params->image_size);

    // initialize input synapses
    for(i = 0; i<snn->n_input; i++){
        //printf("Initializing input synapse %d / %d\n", i+1, snn->n_synapses);
        synapse = &(snn->synapses[i]);

        // TODO
        // IFDEF WEIGHT INCLUDED IN GENOTYPE
        // synapse->weight = synapse_node->weight       
        synapse->delay = 1;
        
        synapse->max_spikes = INPUT_MAX_SPIKES;

        // initialize control variables
        synapse->t_last_post_spike = -1;
        synapse->t_last_pre_spike = -1;

        synapse->post_neuron_computed = -1;
        synapse->pre_neuron_computed = -1;

        synapse->last_spike = 0; 
        synapse->next_spike = 0;

        synapse->l_spike_times = (int *)malloc(synapse->max_spikes * sizeof(int));
        for(j = 0; j<synapse->max_spikes; j++)
            synapse->l_spike_times[j] = -1; // no spikes yet


        // set training rule
        switch (synapse_node->learning_rule) // get synapse training zone from list
        {
            case 0:
                synapse->learning_rule = &add_stdp;//(void (*)())&add_stdp;
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
                synapse->learning_rule = &add_stdp;//(void (*)())&add_stdp;
                break;
        }
    }


    // initialize the rest synapses following synapse nodes
    while(synapse_node != NULL){
        for(j = 0; j<abs(synapse_node->value); j++){
            //printf("Initializing synapse %d / %d\n", i+1, snn->n_synapses);
            synapse = &(snn->synapses[i]);

            // TODO
            // IFDEF WEIGHT INCLUDED IN GENOTYPE
            // synapse->weight = synapse_node->weight      
            
            synapse->delay = synapse_node->latency; // TODO: This should be a list to manage each synapse in a node (a node can contain more than one synapse)
            synapse->max_spikes = MAX_SPIKES;

            // initialize control variables
            synapse->t_last_post_spike = -1;
            synapse->t_last_pre_spike = -1;

            synapse->post_neuron_computed = -1;
            synapse->pre_neuron_computed = -1;

            synapse->last_spike = 0; 
            synapse->next_spike = 0;

            synapse->l_spike_times = (int *)malloc(synapse->max_spikes * sizeof(int));
            for(k = 0; k<synapse->max_spikes; k++)
                synapse->l_spike_times[k] = -1; // no spikes yet


            // set training rule
            switch (synapse_node->learning_rule) // get synapse training zone from list
            {
                case 0:
                    synapse->learning_rule = &add_stdp;//(void (*)())&add_stdp;
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
                    synapse->learning_rule = &add_stdp;//(void (*)())&add_stdp;
                    break;
            }

            i++;
        }

        // move to the next synapse
        synapse_node = synapse_node->next_element;
    } 

    // check that all synapses are initialized
    //printf("i = %d, n_synapses = %d\n", i, snn->n_synapses);
}


void reinitialize_synapses_from_genotype(spiking_nn_t *snn){
    int i;
    synapse_t *synapse;

    for(i = 0; i<snn->n_synapses; i++){
        synapse = &(snn->synapses[i]);

        synapse->t_last_post_spike = -1;
        synapse->t_last_pre_spike = -1;
        
        synapse->post_neuron_computed = -1;
        synapse->pre_neuron_computed = -1;
        
        synapse->last_spike = 0; 
        synapse->next_spike = 0;
    }
}


/* Neurons and synapses connections */

void connect_neurons_by_synapses_from_genotype(spiking_nn_t *snn, individual *ind, NSGA2Type *nsga2Params){
    int next_synapse = 0, i, j, n_synaps;
    int processed_synapses;
    int n_input_synapses = nsga2Params->image_size;
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    // TODO: I don't like too much how this works
    // the first image_size synapses are input synapses
    
    next_synapse = 0;
    
    printf("Joining input synapses and input neurons\n");
    for(i = 0; i<n_input_synapses; i++){
        printf("Neuron / synapse %d\n", i);
        add_input_synapse_to_neuron(snn, i, i);
        next_synapse ++;
    }
    printf("Input synapses and neurons joined!\n");
    
    printf("Joining the rest of neurons and synapses\n");
    while(synapse_node->next_element != NULL){
        for(i = 0; i<abs(synapse_node->value); i++){
            add_input_synapse_to_neuron(snn, synapse_node->col, next_synapse);
            add_output_synapse_to_neuron(snn, synapse_node->row, next_synapse);  
            next_synapse++;
        }

        // move to the next synapse node
        synapse_node = synapse_node->next_element;
    }

    printf("The rest of neurons and synapses joined!\n");
}