#include "snn_library.h"
#include "training_rules/stdp.h"
#include "neuron_models/lif_neuron.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

void initialize_network_neurons(spiking_nn_t *snn, network_construction_lists_t *lists){
    int i, j;

    if(snn->neuron_type == 0){ // lif neurons
        snn->lif_neurons = (lif_neuron_t *)malloc(snn->n_neurons * sizeof(lif_neuron_t)); // reserve memory for lif neurons
    }
    // else {}

    // alloc memory to store input and output synapse amount for each neuron
    int *neurons_input_synapses = malloc(snn->n_neurons * sizeof(int)); // list to count input synapses for each neuron
    int *neurons_output_synapses = malloc(snn->n_neurons * sizeof(int));
    
    // initialize list
    for(i = 0; i<snn->n_neurons; i++){
        neurons_input_synapses[i] = 0;
        neurons_output_synapses[i] = 0;
    }

    // count neuron input synapses
    
    // input layer
    for(i=0; i<lists->synaptic_connections[0][0]; i++){
        neurons_input_synapses[lists->synaptic_connections[0][i*2+1]] += lists->synaptic_connections[0][i*2+2];
    }

    // rest of layers
    for(i=1; i<snn->n_neurons+1; i++){
        for(j=0; j<lists->synaptic_connections[i][0]; j++){
            neurons_input_synapses[lists->synaptic_connections[i][j*2+1]] += lists->synaptic_connections[i][j*2+2];
            neurons_output_synapses[i-1] += lists->synaptic_connections[i][j*2+2];
        }
    }


    /* OLD VERSION, REMOVE IN THE FUTURE
    for(i = 0; i<(snn->n_neurons); i++){ // analyze each row of synaptic connections
        for(j=0; j<(synaptic_connections[i][0]); j++){ // input synapses are included too
            neurons_input_synapses[synaptic_connections[i][j*2+1]] += synaptic_connections[i][j*2+2];
        }
    }

    // count output synapses
    for(i=0; i<(snn->n_neurons); i++){
        for(j=0; j<(synaptic_connections[i+1][0]); j++){ // i+1 as i=0 row values are the network input synapses
            neurons_output_synapses[i] += synaptic_connections[i+1][j*2+2];
        }
    }*/

    // initialize lif neurons
    for(i=0; i<snn->n_neurons; i++){
        if(snn->neuron_type == 0) // lif neuron
            initialize_lif_neuron(snn, i, lists, lists->synaptic_connections, neurons_input_synapses[i], neurons_output_synapses[i]);
    }

    // free memory TODO, THIS GENERATES AN ERROR
    //free(neurons_input_synapses);
    //free(neurons_output_synapses);
}

void initialize_synapse(synapse_t *synapse, network_construction_lists_t *lists, spiking_nn_t *snn, int synapse_id){
    // read synapse parameters from lists
    synapse->w = lists->weight_list[synapse_id];//w;
    synapse->delay = lists->delay_list[synapse_id];//delay;

    // TODO: THIS MUST BE REVISED, IN GENERAL THE WAY I MANAGE THE INPUT
    if(synapse_id < snn->n_input_synapses || synapse_id >= snn->n_synapses - snn->n_output_synapses)//synapse_id > snn->n_output_synapses)
        synapse->max_spikes = INPUT_MAX_SPIKES;
    else    
        synapse->max_spikes = MAX_SPIKES;

    // initialize control parameters
    synapse->t_last_post_spike = -1;
    synapse->t_last_pre_spike = -1;
    
    synapse->post_neuron_computed = -1;
    synapse->pre_neuron_computed = -1;
    
    synapse->last_spike = 0; 
    synapse->next_spike = 0;

    synapse->l_spike_times = (int *)malloc(synapse->max_spikes * sizeof(int));
    for(int i = 0; i<synapse->max_spikes; i++)
        synapse->l_spike_times[i] = -1; // no spikes yet


    // set training rule
    switch (lists->training_zones[synapse_id]) // get synapse training zone from list
    {
    case 0:
        synapse->learning_rule = &add_stdp;//(void (*)())&add_stdp;
        break;
    case 1:
        synapse->learning_rule = &mult_stdp;//(void (*)())&mult_stdp;
        break;
    case 2:
        synapse->learning_rule = &anti_stdp;//(void (*)())&anti_stdp;
        break;
    case 3:
        synapse->learning_rule = &triplet_stdp;//(void (*)())&triplet_stdp;
        break;
    default:
        break;
    }
}

void re_initialize_synapse(synapse_t *synapse, network_construction_lists_t *lists){
    synapse->t_last_post_spike = -1;
    synapse->t_last_pre_spike = -1;
    
    synapse->post_neuron_computed = -1;
    synapse->pre_neuron_computed = -1;
    
    synapse->last_spike = 0; 
    synapse->next_spike = 0;

    // not necessary
    //for(int i = 0; i<synapse->max_spikes; i++)
    //    synapse->l_spike_times[i] = -1; // no spikes yet
}


void initialize_network_synapses(spiking_nn_t *snn, int n_synapses, network_construction_lists_t *lists){
    // reserve memory for synapses
    snn->synapses = (synapse_t *)malloc(n_synapses * sizeof(synapse_t));

    for(int i = 0; i<n_synapses; i++)
        initialize_synapse(&(snn->synapses[i]), lists, snn, i);
}


void add_input_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index){
    if(snn->neuron_type == 0)
        add_input_synapse_to_lif_neuron(&snn->lif_neurons[neuron_index], &(snn->synapses[synapse_index]), synapse_index);
    // else{}
}


void add_output_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index){
    if(snn->neuron_type == 0)
        add_output_synapse_to_lif_neuron(&snn->lif_neurons[neuron_index], &(snn->synapses[synapse_index]), synapse_index);
    // else{}
}

void connect_neurons_and_synapses(spiking_nn_t *snn, int **synaptic_connections){
    int n_neurons = snn->n_neurons;
    int n_input = snn->n_input;
    int n_output = snn->n_output;

    int i, j, l;
    int i_synapse = 0; // index of the actual synapse

    // add network input synapses
    for(i = 0; i<(synaptic_connections[0][0]); i++){ // all this synapses are network input connections
        // 
        for(j=0; j<(synaptic_connections[0][i*2+2]); j++){
            add_input_synapse_to_neuron(snn, synaptic_connections[0][i*2+1], i_synapse);
            i_synapse++;
        }
    }
    
    //i_synapse=0;
    // connect neurons with output and input synapses
    for(i = 0; i<(n_neurons); i++){ 
        for(j = 0; j<(synaptic_connections[i+1][0]); j++){
            if(synaptic_connections[i+1][j*2+1] != -1){ // network output synapse
                for(l=0; l<synaptic_connections[i+1][j*2+2]; l++){
                    add_input_synapse_to_neuron(snn, synaptic_connections[i+1][j*2+1], i_synapse);
                    add_output_synapse_to_neuron(snn, i, i_synapse); // neuron identifier is i, but in file neuron i is located on i+1 row
                    i_synapse++;
                }
            }
        }
    }

    for(i = 0; i<(n_neurons); i++){
        for(j = 0; j<(synaptic_connections[i+1][0]); j++){
            if(synaptic_connections[i+1][j*2+1] == -1){ // network output synapse
                for(l=0; l<synaptic_connections[i+1][j*2+2]; l++){
                    add_output_synapse_to_neuron(snn, i, i_synapse);   
                    i_synapse++;
                }
            }    
        }
    }
}

void initialize_network(spiking_nn_t *snn, simulation_configuration_t *conf, network_construction_lists_t *lists){ //void *neuron_initializer()){
    // initialize general information
    snn->neuron_type = conf->neuron_type;

    // initialize step function depending on neuron type
    switch (snn->neuron_type)
    {
        case 0: // LIF
            snn->complete_step = &lif_neuron_step;
            snn->neuron_initializer = &initialize_lif_neuron;
            snn->neuron_re_initializer = &re_initialize_lif_neuron;
            snn->input_step = &lif_neuron_compute_input_synapses;
	        snn->output_step = &lif_neuron_compute_output_synapses;
	    break;
        case 1: // HH
            break;
        default:
            break;
    }

    // initialize neurons of the network
    initialize_network_neurons(snn, lists);

#ifdef DEBUG
    printf("Neurons initialized\n");
#endif

    // initialize synapses
    initialize_network_synapses(snn, snn->n_synapses, lists);

#ifdef DEBUG
    printf("Synapses initilized\n");
#endif

    // connect neurons and synapses
    connect_neurons_and_synapses(snn, lists->synaptic_connections);
}

void initialize_results_struct(simulation_results_t *results, simulation_configuration_t *conf, spiking_nn_t *snn){
    // initialize struct to store simulation configuration and outputs
    results->elapsed_time = 0;
    results->elapsed_time_neurons = 0;
    results->elapsed_time_synapses = 0;
    results->elapsed_time_neurons_input = 0;
    results->elapsed_time_neurons_output = 0;

    results->generated_spikes = malloc(snn->n_neurons * sizeof(unsigned char *));
    for (int i = 0; i<snn->n_neurons; i++)
        results->generated_spikes[i] = malloc((conf->time_steps) * sizeof(unsigned char));
}

void free_lists_memory(network_construction_lists_t *lists, spiking_nn_t *snn){
    for(int i=0; i<(snn->n_neurons+1); i++){
        free(lists->synaptic_connections[i]);
    }
    free(lists->synaptic_connections);
    free(lists->weight_list);
    free(lists->delay_list);
    free(lists->training_zones);
}