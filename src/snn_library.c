#include "snn_library.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "training_rules/stdp.h"
#include "neuron_models/lif_neuron.h"
#include "encoders/image_encoders.h"

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
            initialize_lif_neuron(snn, i, lists, neurons_input_synapses[i], neurons_output_synapses[i]);
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



void re_initialize_synapses(spiking_nn_t *snn){
    for(int i = 0; i<snn->n_synapses; i++)
        re_initialize_synapse(&(snn->synapses[i]));
}


void re_initialize_synapse(synapse_t *synapse){
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

void initialize_network_function_pointers(spiking_nn_t *snn){
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
    printf("    - Initializing neurons...\n");
    initialize_network_neurons(snn, lists);
    printf("    - Neurons initialized!\n");

#ifdef DEBUG
    printf("Neurons initialized\n");
#endif

    // initialize synapses
    printf("    - Initializing synapses...\n");
    initialize_network_synapses(snn, snn->n_synapses, lists);
    printf("    - Synapses initialized!\n");

#ifdef DEBUG
    printf("Synapses initilized\n");
#endif

    // connect neurons and synapses
    printf("Connecting neuron...\n");
    connect_neurons_and_synapses(snn, lists->synaptic_connections);
    printf("Neurons connected!\n");
}


// This function reorders the list of synapses following the input criterion
void reorder_synapse_list(spiking_nn_t *snn){
    int *new_order_indexes = (int *)malloc(snn->n_synapses * sizeof(int));
    int *map_indexes = (int *)malloc(snn->n_synapses * sizeof(int));

    synapse_t *new_list = (synapse_t *)malloc(snn->n_synapses * sizeof(synapse_t));
    int i, j, l, next_pos = 0, n_synap;

    lif_neuron_t *lif_neuron, *pre_neuron;
    synapse_t *synapse;

    // input synapses are maintained as at the beginning
    for(i=0; i<snn->n_input_synapses; i++){
        new_order_indexes[next_pos] = i;
        map_indexes[i] = next_pos; // store mapping
        next_pos ++;
    }

    // order by NEURONS input synapses
    for(i=0; i<snn->n_neurons; i++){
        //printf("Computing neuron %d\n", i);
        if(snn->neuron_type == 0){ // LIF neurons
            //printf("Neuron %d\n", i);
            lif_neuron = &(snn->lif_neurons[i]);
            n_synap = snn->lif_neurons[i].n_input_synapse;
            for(j=0; j<n_synap; j++){
                if(lif_neuron->input_synapse_indexes[j] >= snn->n_input_synapses){
                    new_order_indexes[next_pos] = lif_neuron->input_synapse_indexes[j]; // store synapse index in the list
                    map_indexes[lif_neuron->input_synapse_indexes[j]] = next_pos; // store mapping
                    next_pos ++;
                }
            }
        }
    }


    // add NETWORK output synapses
    for(i = snn->n_synapses - snn->n_output_synapses; i< snn->n_synapses; i++){
        new_order_indexes[next_pos] = i;
        map_indexes[i] = next_pos; // store mapping
        next_pos ++;
    }
    
    
    // copy the new order
    for(i = 0; i<snn->n_synapses; i++){
        new_list[i] = snn->synapses[new_order_indexes[i]];
    }

    // copy again but in snn structure
    for(i = 0; i<snn->n_synapses; i++){
        snn->synapses[i] = new_list[i];
    }    

    /*for(i = 0; i<snn->n_synapses; i++)
        printf("%d ", new_order_indexes[i]);

    printf("\n");*/

    // use mapping to change 


    for(i=0; i<snn->n_neurons; i++){
        lif_neuron = &(snn->lif_neurons[i]);
        n_synap = snn->lif_neurons[i].n_input_synapse;

        for(j=0; j<n_synap; j++){
            lif_neuron->input_synapse_indexes[j] = map_indexes[lif_neuron->input_synapse_indexes[j]];
        }

        n_synap = snn->lif_neurons[i].n_output_synapse;
        for(j=0; j<n_synap; j++){
            lif_neuron->output_synapse_indexes[j] = map_indexes[lif_neuron->output_synapse_indexes[j]];
        }
    }

    /*printf("Index lists by neurons\n");
    for(i=0; i<snn->n_neurons;i++){
        printf("    - Printing neuron %d lists\n", i);
        lif_neuron = &(snn->lif_neurons[i]);

        printf("        -");
        for(j=0; j<lif_neuron->n_input_synapse; j++){
            printf("%d ,", lif_neuron->input_synapse_indexes[j]);
        }
        printf("\n");

        printf("        -");
        for(j=0; j<lif_neuron->n_output_synapse; j++){
            printf("%d ,", lif_neuron->output_synapse_indexes[j]);
        }
        printf("\n");
    }*/

    free(new_list);
    free(new_order_indexes);
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
    int i;

    for(i=0; i<(snn->n_neurons+1); i++){
        free(lists->synaptic_connections[i]);
    }
    free(lists->synaptic_connections);
    free(lists->weight_list);
    free(lists->delay_list);
    free(lists->training_zones);
}


void print_lif_neuron_information(lif_neuron_t *lif_neuron){
    int i;
    
    printf("      > Excitatory (1) / inhibitory (0) = %d; ", lif_neuron->excitatory);
    printf("Neuron membrane potential = %f; ", lif_neuron->v);
    printf("Neuron membrane potential threshold = %f; ", lif_neuron->v_tresh);
    printf("Neuron resting membrane potential = %f; ", lif_neuron->v_rest);
    printf("Neuron resistance = %f; ", lif_neuron->r);
    printf("Neuron refractory time = %d; ", lif_neuron->r_time);
    printf("Neuron is input = %d; ", lif_neuron->is_input_neuron);
    printf("Neuron is output = %d; ", lif_neuron->is_output_neuron);
    printf("Neuron number of input synapses = %d; ", lif_neuron->n_input_synapse);
    printf("Neuron number of output synapses = %d\n", lif_neuron->n_output_synapse);
    printf("      > Neuron input synapse indexes = ");
    for(i=0; i<lif_neuron->n_input_synapse; i++){
        printf("%d ", lif_neuron->input_synapse_indexes[i]);
    }
    printf("\n");

    printf("      > Neuron output synapse indexes = ");
    for(i=0; i<lif_neuron->n_output_synapse; i++){
        printf("%d ", lif_neuron->output_synapse_indexes[i]);
    }
    printf("\n");
    printf("\n");
}

void print_lif_neurons_information(spiking_nn_t *snn){
    int i;
    
    for(i = 0; i<snn->n_neurons; i++){
        print_lif_neuron_information(&(snn->lif_neurons[i]));
    }
}

void print_neurons_information(spiking_nn_t *snn){
    switch(snn->neuron_type)
    {
        case 0:
            print_lif_neurons_information(snn);
            break;
        default:
            print_lif_neurons_information(snn);
            break;
    }
}


void print_input_synapse_information(synapse_t *synapse){
    printf("        > Weight = %f; ", synapse->w);
    printf("Delay = %d\n", synapse->delay);
}

void print_synapse_information(synapse_t *synapse){
    printf("        > Weight = %f; ", synapse->w);
    printf("Delay = %d\n", synapse->delay);
}

void print_output_synapse_information(synapse_t *synapse){
    printf("        > Weight = %f; ", synapse->w);
    printf("Delay = %d\n", synapse->delay);
}

void print_synapses_information(spiking_nn_t *snn){
    int i;

    // print input synapses
    printf("      > Printing input synapses:\n");
    for(i=0; i<snn->n_input_synapses; i++){
        print_input_synapse_information(&(snn->synapses[i]));
    }
    printf("\n");

    // print synapses (not input or output)
    printf("      > Printing middle synapses:\n");
    for(i = snn->n_input_synapses; i<snn->n_synapses - snn->n_output_synapses; i++){
        print_synapse_information(&(snn->synapses[i]));
    }
    printf("\n");

    // print output synapses
    printf("       > Printing output synapses:\n");
    for(i=snn->n_synapses - snn->n_output_synapses; i<snn->n_synapses; i++){
        print_output_synapse_information(&(snn->synapses[i]));
    }
    printf("\n");
}

void print_network_information(spiking_nn_t *snn){
    printf("    > Printing general information:\n");
    printf("      > Type of the neurons = %d\n", snn->neuron_type);
    printf("      > Number of neurons = %d\n", snn->n_neurons);
    printf("      > Number of input neurons = %d\n", snn->n_input);
    printf("      > Number of output neurons = %d\n", snn->n_output);
    printf("      > Number of synapses = %d\n", snn->n_synapses);
    printf("      > Number of input synapses = %d\n", snn->n_input_synapses);
    printf("      > Number of output synapses = %d\n\n", snn->n_output_synapses);

    printf("    > Printing neurons information:\n");
    print_neurons_information(snn);
    printf("    > All neurons printed!\n\n");

    //printf("    > Printing synapses information:\n");
    //print_synapses_information(snn);
    //printf("    > All synapses printed!\n\n");

    
}