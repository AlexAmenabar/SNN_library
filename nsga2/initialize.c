/* Data initializtion routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h" 

// This function initializes the motifs of the individual
void initialize_ind_motifs(individual *ind){
    int i;
    int motif_type;

    for(i = 0; i<ind->n_motifs; i++){
        motif_type = rnd(0, n_motifs-1);
        ind->motifs[i].motif_type = motif_type;
        ind->motifs[i].initial_global_index = ind->n_neurons; // index of the first neuron in the neuron list

        // count neurons and synapses
        ind->n_neurons += motifs[motif_type].n_neurons;
        ind->n_synapses += motifs[motif_type].n_synapses;
    }
}


/**
Functions related to neuron nodes initialization
*/

// Initialize neuron node
// TODO: this function should be more general and initialize neuron with some randomization
void initialize_neuron_node(neuron_node_t *neuron_node){
    neuron_node->threshold = 150;
    neuron_node->refract_time = 3;
    neuron_node->v_rest = 0;
    neuron_node->r = 10;
}

// Function to initialize all neuron nodes
void initialize_neuron_nodes(individual *ind){
    int i;
    neuron_node_t *neuron_node;


    // allocate memory for first element of the neurons list
    ind->neurons = (neuron_node_t *)malloc(sizeof(neuron_node_t));

    // initialize first neuron
    neuron_node = ind->neurons;
    initialize_neuron_node(neuron_node);

    // loop to initialize the rest of the neurons
    for(i = 1; i<ind->n_neurons; i++){        
        // allocate memory for next neuron
        neuron_node->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t));
        neuron_node = neuron_node->next_neuron;

        // initialize neuron
        initialize_neuron_node(neuron_node);
    }
}


/**
Functions to connect motifs and construct the sparse matrix
*/



void connect_motif_all_outputs_2_motif_all_inputs(){

}



void construct_sparse_matrix(individual *ind, int *n_selected_motifs_per_motif, int **selected_motifs_per_motif){
    int i, j, k, l, s, neuron_index, temp_neuron_index, actual_motif_index, temp_motif_index;
    motif_node_t *actual_motif, *temp_motif;
    motif_t *actual_motif_info, *temp_motif_info;
    sparse_matrix_node_t *matrix_node;

    // initialize connectivity matrix
    ind->connectivity_matrix = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = ind->connectivity_matrix;

    //TODO
    // initialize matrix node (should be initialized in a function)
    matrix_node->row = -1;
    matrix_node->col = -1;
    matrix_node->value = 0;
    matrix_node->latency = 3;

    /*
    To create the sparse matrix for each matrix
        - Loop over all motifs (set of rows)
            - For each motif loop over its neurons (each neuron is a row)
                - For each loop over motifs connected to the actual motif
    */

    // loop over motifs
    for(i=0; i<ind->n_motifs; i++){

        // get information about the motif
        actual_motif = &(ind->motifs[i]); // get the motif
        actual_motif_index = i; // store the index of the motif
        neuron_index = actual_motif->initial_global_index; // get the GLOBAL index of the first neuron of the motif
        actual_motif_info = &(motifs[actual_motif->motif_type]); // get motif information from motif definition list

        /*for(j = 0; j<n_selected_motifs_per_motif[i]; j++){
            printf("%d, ", selected_motifs_per_motif[i][j]);
        }
        printf("\n");*/


        // create each row of the motif looping over the neurons of the motif
        for(j = 0; j<actual_motif_info->n_neurons; j++){ 

            int actual_motif_processed = 0; // used to control if the actual motif has been processed

            // add the connections with the rest of the neurons of the connected motifs
            for(s = 0; s<n_selected_motifs_per_motif[i]; s++){

                //printf("        - In selected motif %d (%d)\n", s, selected_motifs_per_motif[i][s]);

                temp_motif_index = selected_motifs_per_motif[i][s];
                temp_motif = &(ind->motifs[temp_motif_index]);
                temp_neuron_index = temp_motif->initial_global_index;
                temp_motif_info = &(motifs[temp_motif->motif_type]);

                // count how much times this motif appears (the actual motif can be connected more than ones to the same motif)
                int count = 1;

                if(s < n_selected_motifs_per_motif[i] - 1){
                    int equals = 1;
                    int s2 = s + 1;
                    int next_motif_index;

                    while(equals == 1 && s2 < n_selected_motifs_per_motif[i]){
                        next_motif_index = selected_motifs_per_motif[i][s2];
                        if(next_motif_index != temp_motif_index)
                            equals = 0;
                        else{
                            count++;
                            s2++;
                            s++;
                        }
                    }
                }


                // if temporal motif index is lower than the actial motif, add connections with that motif
                if(temp_motif_index < actual_motif_index || temp_motif_index > actual_motif_index && actual_motif_processed == 1){
                    // add connections with that motif (only for output neurons)

                    // if the actual neuron is a output neuron in the motif, connect to temporal motif input neurons
                    for(k=0; k<actual_motif_info->n_output; k++){
                        if(j == actual_motif_info->output_neurons[k]){ // is an output neuron

                            // connect to all temp motif input neurons
                            for(l = 0; l<temp_motif_info->n_input; l++){
                                // row and column depends on neuron global index
                                matrix_node->row = neuron_index + j;
                                matrix_node->col = temp_neuron_index + temp_motif_info->input_neurons[l]; // column depends on the motif to connect with

                                matrix_node->value += count; // TODO: sum the amount of connections
                                matrix_node->latency = 3; // TODO: this should depend on an exponential distribution

                                // allocate memory for the next matrix_node
                                matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
                                matrix_node = matrix_node->next_element;
                            }
                        }
                    }

                    // if the selected motif is the last, and the motif internal structure has not been added, do it now
                    if(s == n_selected_motifs_per_motif[i]-1 && actual_motif_processed == 0){
                        actual_motif_processed = 1;
                        for(k=0; k<actual_motif_info->n_neurons; k++){
                            if(actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k] != 0){ // there is a connection
                                // row and column depends on neuron global index

                                matrix_node->row = neuron_index + j;
                                matrix_node->col = neuron_index + k;
                                matrix_node->value += actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k]; // TODO: I think this shouldn't be a fixed value
                                matrix_node->latency = 3; // TODO: this should depend on an exponential distribution

                                // allocate memory for the next matrix_node
                                matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
                                matrix_node = matrix_node->next_element;
                            }
                        }
                    }
                }
                
                // if is bigger or equals, and the actual motif has not been processed, process it
                else { // if(temp_motif_index >= actual_motif_index && actual_motif_processed == 0){
                    

                    // internal connections of the motif must be added

                    if(temp_motif_index == actual_motif_index){
                        // add motif internal info and external at the same time if it is necessary
                        
                        for(k=0; k<actual_motif_info->n_neurons; k++){
                        
                            int value = 0;

                            // check if there is a connection depending on motif internal strucutre
                            if(actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k] != 0){ // there is a connection
                                value += actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k];
                            }

                            // add the connection of the motif with itself if it is necessary
                            //check if j. neuron is an output neuron in actual motif
                            for(l = 0; l<actual_motif_info->n_output; l++){
                                if(j == actual_motif_info->output_neurons[l]){
                                    // check if k is an input neurons
                                    for(int t = 0; t<actual_motif_info->n_input; t++){
                                        if(actual_motif_info->input_neurons[t] == k){
                                            if(value>=0)
                                                value += 1;
                                            else
                                                value -= 1;
                                        }
                                    }
                                }
                            }

                            // row and column depends on neuron global index
                            if(value != 0){ // add element only if there are connections
                                matrix_node->row = neuron_index + j;
                                matrix_node->col = neuron_index + k;
                                matrix_node->value += value; // TODO: I think this shouldn't be a fixed value
                                matrix_node->latency = 3; // TODO: this should depend on an exponential distribution

                                // allocate memory for the next matrix_node
                                matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
                                matrix_node = matrix_node->next_element;
                            }
                        }
                    }

                    // 

                    else{ // temp_motif_index > actual_motif_index
                        // add first motif internal info
                        for(k=0; k<actual_motif_info->n_neurons; k++){
                            //printf("                    - k = %d...\n", k);
                            if(actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k] != 0){ // there is a connection
                                // row and column depends on neuron global index

                                matrix_node->row = neuron_index + j;
                                matrix_node->col = neuron_index + k;
                                matrix_node->value += actual_motif_info->connectivity_matrix[j * actual_motif_info->n_neurons + k]; // TODO: I think this shouldn't be a fixed value
                                matrix_node->latency = 3; // TODO: this should depend on an exponential distribution

                                // allocate memory for the next matrix_node
                                matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
                                matrix_node = matrix_node->next_element;
                            }
                        }

                        // add information of connections with the other motif
                        // if the actual neuron is a output neuron in the motif, connect to temporal motif input neurons
                        for(k=0; k<actual_motif_info->n_output; k++){
                            if(j == actual_motif_info->output_neurons[k]){ // is an output neuron

                                // connect to all temp motif input neurons
                                for(l = 0; l<temp_motif_info->n_input; l++){
                                    // row and column depends on neuron global index
                                    matrix_node->row = neuron_index + j;
                                    matrix_node->col = temp_neuron_index + temp_motif_info->input_neurons[l]; // column depends on the motif to connect with

                                    matrix_node->value += count; // TODO: sum the amount of connections
                                    matrix_node->latency = 3; // TODO: this should depend on an exponential distribution

                                    // allocate memory for the next matrix_node
                                    matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
                                    matrix_node = matrix_node->next_element;
                                }
                            }
                        }
                    }

                    // actual motif has been processed
                    actual_motif_processed = 1;
                }            
            }
        }
    }

    // free last element of the sparse matrix and set the pointer to NULL
    free(matrix_node->next_element);

    matrix_node->next_element = NULL;
    matrix_node = NULL;
}


// This function "connect the motifs"
void connect_motifs(individual *ind){
    int *n_connections_per_motif; // number of connections between motifs per each motif
    int **selected_motifs_per_motif; // list of motifs to connect with each motif. 
                                    // For each motif it indicates to which motifs of the ind->motifs list is connected the motif (indexes in the list)
    int i, j, n_connections;
    motif_node_t *actual_motif;

    n_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int));
    selected_motifs_per_motif = (int **)malloc(ind->n_motifs * sizeof(int *));

    // TODO: This should be refactorized to a function, I think
    // create connections between motifs
    for(i = 0; i<ind->n_motifs; i++){
        
        // select how much connections each motif has (connections motif - motif)
        n_connections = 2; // TODO: This must be a random value, probably a random number following an exponential distribution
        n_connections_per_motif[i] = n_connections;

        // allocate memory
        selected_motifs_per_motif[i] = (int *)malloc(n_connections * sizeof(int));

        // select n_connections motifs to connect i.th motif with
        for(j = 0; j<n_connections; j++){
            // select a random motif to connect with
            selected_motifs_per_motif[i][j] = rnd(0, ind->n_motifs-1); // TODO: restrictions (some motif types can not be connected to other types...)
        }

        // order the list (lower to higher)
        insertion_sort(selected_motifs_per_motif[i], n_connections);
    }

    // construct sparse connections matrix from connections between motifs
    construct_sparse_matrix(ind, n_connections_per_motif, selected_motifs_per_motif);


    // free memory
    for(i = 0; i<ind->n_motifs; i++){
        free(selected_motifs_per_motif[i]);
    }
    free(selected_motifs_per_motif);
    free(n_connections_per_motif);
}



/**
Population init general functions
*/

/*
    TODO: SOME NOTES FOR FUTURE CHANGES
        - NOW ALL MOTIF OUTPUT NEURONS ARE CONNECTED TO SELECTED MOTIFS ALL INPUTS: THIS SHOULD BE CHANGED IN THE FUTURE TO MORE SPECIFIC APPROACHES
        - INPUT SYNAPSES ARE ADDED IN A CUTRE WAY, REVISE IN THE FUTURE
        - It should be helpfull to set restrictions: for example, type 4 motif can not be connected to type 4 motif... and things like that
*/



/* Function to initialize a population randomly */
void initialize_pop (NSGA2Type *nsga2Params, population *pop)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        initialize_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to initialize an individual randomly (memory is allocated here too)*/
void initialize_ind (NSGA2Type *nsga2Params, individual *ind)
{
    neuron_node_t *neuron;
    int neuron_index;

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = rnd(nsga2Params->min_motifs, nsga2Params->max_motifs); // random number of motifs
    
    // allocate memory to store individual motifs info
    ind->motifs = (motif_node_t *)malloc(ind->n_motifs * sizeof(motif_node_t)); // allocate memory to store motifs
    
    // initialize number of neurons and synapses of the individual to 0
    ind->n_neurons = 0;
    ind->n_synapses = 0;


    // initialize motifs
    initialize_ind_motifs(ind);


    // initialize neurons
    initialize_neuron_nodes(ind);


    // connect motifs and construct sparse matrix
    connect_motifs(ind);


    // allocate memory for spiking neural network structure
    ind->snn = (spiking_nn_t *)malloc(sizeof(spiking_nn_t));
    
    return;
}
