/* Data initializtion routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h" 


/**
Functions related to neuron nodes initialization
*/

// Initialize neuron node
// TODO: this function should be more general and initialize neuron with some randomization
void initialize_neuron_node(neuron_node_t *neuron_node){
    neuron_node->threshold = 150;
    neuron_node->refract_time = 3;
    neuron_node->v_rest = 0;
    neuron_node->r = 10.0;
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

// THIS FUNCTION IS NEW
// This function initializes the motifs of the individual
void initialize_motif_node(new_motif_t *motif, int motif_id, individual *ind){
    // initialize motif data
    motif->motif_id = motif_id;
    motif->motif_type = rnd(0, n_motifs-1);
    motif->initial_global_index = ind->n_neurons;

    // add the amount of synapses of the motif to the individual
    ind->n_neurons += motifs_data[motif->motif_type].n_neurons;
}


void initialize_ind_motifs(individual *ind){
    int i;
    int motif_type;
    new_motif_t *motif_node; // NEW

    // OLD
    /*for(i = 0; i<ind->n_motifs; i++){
        motif_type = rnd(0, n_motifs-1);
        ind->motifs[i].motif_type = motif_type;
        ind->motifs[i].initial_global_index = ind->n_neurons; // index of the first neuron in the neuron list

        // count neurons and synapses
        ind->n_neurons += motifs[motif_type].n_neurons;
        ind->n_synapses += motifs[motif_type].n_synapses;
    }*/

    // NEW

    // initialize number of neurons and synapses of the individual to 0
    ind->n_neurons = 0;
    ind->n_input_neurons = 0;

    ind->n_synapses = 0;
    ind->n_input_synapses = 0;

    // allocate memory for the first motif
    ind->motifs_new = (new_motif_t *)malloc(sizeof(new_motif_t));
    motif_node = ind->motifs_new;

    initialize_motif_node(motif_node, 0, ind);

    // initialize the rest of motifs
    for(i = 1; i<ind->n_motifs; i++){
        // allocate memory
        motif_node->next_motif = (new_motif_t *)malloc(sizeof(new_motif_t));
        motif_node = motif_node->next_motif;
        initialize_motif_node(motif_node, i, ind);
    }
}

// TODO: TEST THIS FUNCTION
/* Functions to connect motifs and neurons */
void connect_motifs_and_neurons(individual *ind){
    int i,j;
    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    motif_node = ind->motifs_new;
    neuron_node = ind->neurons;

    // connect the first one
    motif_node->first_neuron = neuron_node;

    // for each motif find the first neuron using the number of neuron for each motif
    for(i = 1; i<ind->n_motifs; i++){
        motif_node = motif_node->next_motif;

        // advance the amount of neurons of the actual motif type
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++){
            neuron_node = neuron_node->next_neuron;
        }
        motif_node->first_neuron = neuron_node;
    }
}


/**
Functions to connect motifs and construct the sparse matrix
*/

/* This function initializes the matrix node */
sparse_matrix_node_t* initialize_sparse_matrix_node(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    int i;

    matrix_node->value = value;
    matrix_node->row = row;
    matrix_node->col = col;

    matrix_node->latency = (int8_t *)malloc(abs(value) * sizeof(int8_t));
    matrix_node->weight = (double *)malloc(abs(value) * sizeof(double));
    matrix_node->learning_rule = (int8_t *)malloc(abs(value) * sizeof(int8_t));

    for(i = 0; i<abs(value); i++){
        matrix_node->latency[i] = 3; // TODO: This must be a random value following an exponential distribution
        matrix_node->weight[i] = 0; // TODO: In this moment weight is not used inside the evolutionary algorithm, it is here for future purposes
        matrix_node->learning_rule[i] = 0; // TODO: Learning rule is not initialized in the first phase of the evolutionary algorithm
    }

    // allocate memory for the next node and move to it
    matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = matrix_node->next_element;

    // TODO: This should be in another place
    // add amount of synapses to the individual number of synapses
    ind->n_synapses += abs(value);

    return matrix_node;
}


/* This function adds the nodes representing the motif internal structure to the dynamic list */
sparse_matrix_node_t* build_motif_internal_structure_column_by_input(individual *ind, sparse_matrix_node_t *matrix_node, int motif_type, int global_neuron_index, int neuron_local_index){
    int i, row, col, value;
    motif_t *motif_info;

    motif_info = &(motifs_data[motif_type]);

    // loop over the neuron_index column to find the input neurons
    for(i = 0; i<motif_info->n_neurons; i++){
        if(motif_info->connectivity_matrix[i * motif_info->n_neurons + neuron_local_index] != 0){ // there is a connection at least
            row = global_neuron_index + i;
            col = global_neuron_index + neuron_local_index;
            value = motif_info->connectivity_matrix[i * motif_info->n_neurons + neuron_local_index];
            matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
        }
    }

    return matrix_node;
}

void construct_sparse_matrix3(individual *ind, int *n_selected_input_motifs_per_motif, int **selected_input_motifs_per_motif){
    int i, j, k, s;
    int row, col, value;

    // The following two lists are auxiliary lists used to store motifs types and motifs first neurons global indexes as the dynamic list can not be
    // indexed
    int *motifs_types_list;
    int *motifs_neurons_global_index_list;

    // Other aux variables
    new_motif_t *motif_node, *tmp_motif_node;
    motif_t *motif_info, *tmp_motif_info;
    sparse_matrix_node_t *matrix_node;
    int motif_type, first_neuron_global_index, neuron_index, tmp_motif_type, tmp_first_neuron_global_index, tmp_motif_index, tmp_neuron_index;
    int counter = 0;
    int actual_motif_processed = 0;


    /* Variables initialization */

    // get first motif node
    motif_node = ind->motifs_new;
    tmp_motif_node = motif_node;

    // initialize first sparse matrix node
    ind->connectivity_matrix = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = ind->connectivity_matrix;


    // allocate memory for aux lists and initialize
    motifs_types_list = (int *)malloc(ind->n_motifs * sizeof(int));
    motifs_neurons_global_index_list = (int *)malloc(ind->n_motifs * sizeof(int));
    for(i = 0; i<ind->n_motifs; i++){
        motifs_types_list[i] = tmp_motif_node->motif_type; // store the motif type
        motifs_neurons_global_index_list[i] = tmp_motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        tmp_motif_node = tmp_motif_node->next_motif;
    }

    /* Loop over individual motifs to construct the sparse matrix. The matrix is constructed column by column in an ordered way*/
    for(i=0; i<ind->n_motifs; i++){
        // Get data about the motif that is being processed
        motif_type = motifs_types_list[i]; // get the motif type
        motif_info = &(motifs_data[motif_type]); // get the structure with the motif information
        first_neuron_global_index = motifs_neurons_global_index_list[i]; // get the global index of the first neuron of the motif
        
        /* Loop over the neurons of the motif (j is used as the local index of the neuron inside the motif)*/
        for(j = 0; j<motif_info->n_neurons; j++){ 
            neuron_index = first_neuron_global_index + j; // get the global index of the neuron being computed
            actual_motif_processed = 0; // the actual motif internal structure data has not been added to the sparse matrix

            /* Loop over the motifs connected to the actual one to construct it column by column */
            for(k = 0; k<n_selected_input_motifs_per_motif[i]; k++){
                // Get the information about this motif
                tmp_motif_index = selected_input_motifs_per_motif[i][k]; // get temp motif index in the motif list
                tmp_motif_type = motifs_types_list[tmp_motif_index];
                tmp_motif_info = &(motifs_data[tmp_motif_type]);
                tmp_first_neuron_global_index = motifs_neurons_global_index_list[tmp_motif_index];
                
                // count how much times this motif is connected to the one being computed
                counter = 1;
                while(k < n_selected_input_motifs_per_motif[i] - 1 && 
                    selected_input_motifs_per_motif[i][k] == selected_input_motifs_per_motif[i][k+1]){ // if we are in the last it can no be repited
                        counter ++; // there is a repetition
                        k++; // move forward in k
                }

                // As the temporal motif index in the list of motifs is higher than the one being computed, and the actual motif information
                // has not been added to the matrix, add it now
                if(tmp_motif_index > i && actual_motif_processed == 0){
                    matrix_node = build_motif_internal_structure_column_by_input(ind,matrix_node, motif_type, first_neuron_global_index, j);
                    actual_motif_processed = 1; // actual motif has been processed
                }

                // the motif is connected with itself, so add internal and external connections at the same time
                if(tmp_motif_index == i){ // actual_motif_processed == 0
                    actual_motif_processed = 1;

                    // process the motif internal structure and the connections between motifs at the same time (motif connected with itself)
                    // loop over the actual neuron column looking for connections
                    for(s = 0; s<motif_info->n_neurons; s++){
                        value = motif_info->connectivity_matrix[s * motif_info->n_neurons + j]; // check if there is a connection in the internal structure

                        // check if the actual one is the input neuron and the other an output neurons
                        if(check_if_neuron_is_input(motif_type, j) == 1 && check_if_neuron_is_output(tmp_motif_type, s) == 1){
                            if(value < 0) value -= counter;
                            else if (value > 0) value += counter;
                            else value = counter;
                        }

                        // if there is any connection, add to the list
                        if(value != 0){
                            row = first_neuron_global_index + s;
                            col = first_neuron_global_index + j;
                            matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                        }
                    }
                }
                // Process the connection between motif tmp and the actual one
                else{
                    // check if the actual neuron is an input neuron
                    if(check_if_neuron_is_input(motif_type, j) == 1){
                        // loop over tmp motif neurons and check if they are output
                        for(s = 0; s<tmp_motif_info->n_neurons; s++){
                            if(check_if_neuron_is_output(tmp_motif_type, s) == 1){
                                value = counter;
                                row = tmp_first_neuron_global_index + s;
                                col = first_neuron_global_index + j;
                                matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                            }
                        }
                    }
                }
            }

            // actual motif has not been processed yet, so dot it now
            if(actual_motif_processed == 0)
                matrix_node = build_motif_internal_structure_column_by_input(ind, matrix_node, motif_type, first_neuron_global_index, j);
        }
    }
    
    // free memory
    free(motifs_types_list);
    free(motifs_neurons_global_index_list);
}

void construct_sparse_matrix2(individual *ind, int *n_selected_input_motifs_per_motif, int **selected_input_motifs_per_motif){
    int i, j, k, s, l;
    int row, col, value;

    // The following two lists are auxiliary lists used to store motifs types and motifs first neurons global indexes
    int *motifs_types_list;
    int *motifs_neurons_global_index_list;

    new_motif_t *motif_node, *tmp_motif_node;
    motif_t *motif_info, *tmp_motif_info;
    sparse_matrix_node_t *matrix_node;
    int motif_type, first_neuron_global_index, neuron_index, tmp_motif_type, tmp_first_neuron_global_index, tmp_motif_index, tmp_neuron_index;
    int counter = 0;
    int actual_motif_processed = 0;

    /* Some initializations */

    // get first motif node
    motif_node = ind->motifs_new;
    tmp_motif_node = motif_node;

    // initialize first sparse matrix node
    ind->connectivity_matrix = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = ind->connectivity_matrix;

    // initialize the amount of synapses of the individual
    ind->n_synapses = 0;

    // allocate memory for aux lists and initialize
    motifs_types_list = (int *)malloc(ind->n_motifs * sizeof(int));
    motifs_neurons_global_index_list = (int *)malloc(ind->n_motifs * sizeof(int));
    for(i = 0; i<ind->n_motifs; i++){
        motifs_types_list[i] = tmp_motif_node->motif_type; // store the motif type
        motifs_neurons_global_index_list[i] = tmp_motif_node->initial_global_index; // store the motif's first neuron global index

        tmp_motif_node = tmp_motif_node->next_motif;
    }

    printf("Printing...\n");
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d, ", motifs_neurons_global_index_list[i]);
    }
    printf("\n");

    /* Loop over individual motifs to construct the sparse matrix. The matrix is constructed in columns order to follow the input criterion for neurons
       and get better performance in simulation as cache use is better */
    for(i=0; i<ind->n_motifs; i++){
        // get motif info from motif type
        motif_type = motifs_types_list[i];
        motif_info = &(motifs_data[motif_type]);
        first_neuron_global_index = motifs_neurons_global_index_list[i];
        

        /* Loop over the neurons of the motif (j is the local index of the neurons inside the motif) */
        for(j = 0; j<motif_info->n_neurons; j++){  // TODO: Check if the order of this loop and the next one can be changed
            neuron_index = first_neuron_global_index + j;
            actual_motif_processed = 0; // actual motif has not been added to the sparse matrix yet


            /* Loop over the motifs connected to the actual one to construct it column by column */

            for(k = 0; k<n_selected_input_motifs_per_motif[i]; k++){

                // get motif information
                tmp_motif_index = selected_input_motifs_per_motif[i][k]; // get temp motif index in the motif list
                tmp_motif_type = motifs_types_list[tmp_motif_index];
                tmp_motif_info = &(motifs_data[tmp_motif_type]);
                tmp_first_neuron_global_index = motifs_neurons_global_index_list[tmp_motif_index];
                
                // count if the actual connected motif appears more than once in the lsit
                counter = 1;
                while(k < n_selected_input_motifs_per_motif[i] - 1 && 
                    selected_input_motifs_per_motif[i][k] == selected_input_motifs_per_motif[i][k+1]){ // if we are in the last it can no be repited
                        counter ++; // there is a repetition
                        k++; // move forward in k
                }

                /* 
                Construct the matrix. There are different cases. The way to construct the internal structure of a motif is different to
                the process of building the connections between motifs, as in the first case the internal information is used.

                The sparse matrix is constructed in a way to be ordered by columns, so it is important to take into account how to manage the 
                internal and external connections.
                */

                // build first the internal structure and then the connection
                if(tmp_motif_index > i){
                    // if actual motif is not processed yet, do it now
                    if(actual_motif_processed == 0)
                        matrix_node = build_motif_internal_structure_column_by_input(ind, matrix_node, motif_type, first_neuron_global_index, j);

                    actual_motif_processed = 1; // actual motif has been processed
                    
                    //printf("Internal structure builded\n");
                    // build the connection between motifs: connect selected motifs output neurons to actual motif input neurons
                    
                    // check if the actual neuron is an input neuron
                    if(check_if_neuron_is_input(motif_type, j) == 1){

                        // loop over tmp motif neurons and check if they are output
                        for(s = 0; s<tmp_motif_info->n_neurons; s++){
                            if(check_if_neuron_is_output(tmp_motif_type, s) == 1){
                                value = counter;
                                row = tmp_first_neuron_global_index + s;
                                col = first_neuron_global_index + j;
                                matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                            }
                        }
                    }
                }
                // the motif is connected with itself, so process internal and external connections at the same time
                else if(tmp_motif_index == i){ // actual_motif_processed == 0
                    actual_motif_processed = 1;

                    // process the motif internal structure and the connections between motifs at the same time (motif connected with itself)
                    // loop over the actual neuron column looking for connections
                    for(s = 0; s<motif_info->n_neurons; s++){
                        // check if there is a connection in the internal structure
                        value = motif_info->connectivity_matrix[s * motif_info->n_neurons + j];

                        // check if the actual one is the input neuron and the other an output neurons
                        if(check_if_neuron_is_input(motif_type, j) == 1 && check_if_neuron_is_output(tmp_motif_type, s) == 1){
                            if(value < 0) value -= counter;
                            else if (value > 0) value += counter;
                            else value = counter;
                        }

                        // if there is any connection, add to the list
                        if(value != 0){
                            row = first_neuron_global_index + s;
                            col = first_neuron_global_index + j;
                            matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                        }
                    }
                }
                // process connection between motifs
                else{ // actual_motif_processed == 0
                    // check if the actual neuron is an input neuron
                    if(check_if_neuron_is_input(motif_type, j) == 1){

                        // loop over tmp motif neurons and check if they are output
                        for(s = 0; s<tmp_motif_info->n_neurons; s++){
                            if(check_if_neuron_is_output(tmp_motif_type, s) == 1){
                                value = counter;
                                row = tmp_first_neuron_global_index + s;
                                col = first_neuron_global_index + j;
                                matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                            }
                        }
                    }
                }
            }

            // actual motif not processed yet, dot it now
            if(actual_motif_processed == 0)
                matrix_node = build_motif_internal_structure_column_by_input(ind, matrix_node, motif_type, first_neuron_global_index, j);
        }
    }
}

/**
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

/*    // loop over motifs
    for(i=0; i<ind->n_motifs; i++){

        // get information about the motif
        actual_motif = &(ind->motifs[i]); // get the motif
        actual_motif_index = i; // store the index of the motif
        neuron_index = actual_motif->initial_global_index; // get the GLOBAL index of the first neuron of the motif
        actual_motif_info = &(motifs_data[actual_motif->motif_type]); // get motif information from motif definition list

        /*for(j = 0; j<n_selected_motifs_per_motif[i]; j++){
            printf("%d, ", selected_motifs_per_motif[i][j]);
        }
        printf("\n");*/


/*        // create each row of the motif looping over the neurons of the motif
        for(j = 0; j<actual_motif_info->n_neurons; j++){ 

            int actual_motif_processed = 0; // used to control if the actual motif has been processed

            // add the connections with the rest of the neurons of the connected motifs
            for(s = 0; s<n_selected_motifs_per_motif[i]; s++){

                //printf("        - In selected motif %d (%d)\n", s, selected_motifs_per_motif[i][s]);

                temp_motif_index = selected_motifs_per_motif[i][s];
                temp_motif = &(ind->motifs[temp_motif_index]);
                temp_neuron_index = temp_motif->initial_global_index;
                temp_motif_info = &(motifs_data[temp_motif->motif_type]);

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
}*/


// This function "connect the motifs"
void connect_motifs(individual *ind){
    int i, j, tmp_motif, valid;
    int n_change, r_motif, n_con;

    int n_connections, max_connect_per_motif;

    int *n_input_connections_per_motif; // number of input connections between motifs per each motif
    int *n_output_connections_per_motif; // number of output connections of the actual motif with other motifs

    int *n_input_connections_per_motif_done; // number of input connections between motifs per each motif already done
    int *n_output_connections_per_motif_done; // number of output connections of the actual motif with other motifs already done

    int **selected_input_motifs_per_motif; // list of motifs to connect with each motif. 
    int **selected_output_motifs_per_motif; // list of motifs to connect with each motif. 


    /* allocate memory for lists */
    n_input_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int));
    n_output_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int));

    n_input_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int));
    n_output_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int));

    selected_input_motifs_per_motif = (int **)malloc(ind->n_motifs * sizeof(int *));
    selected_output_motifs_per_motif = (int **)malloc(ind->n_motifs * sizeof(int *));


    /* set the total number of connections between motifs */
    max_connect_per_motif = rnd(2, 5); // this is done like this to get a divisible number by 

    printf("\n\nN motifs = %d, max connect per motif = %d\n", ind->n_motifs, max_connect_per_motif);

    /* Initialize the lists */
    for(i = 0; i<ind->n_motifs; i++){
        // set the amount of input and output connections for the motif
        n_input_connections_per_motif[i] = max_connect_per_motif; // I am not very convinced with this
        n_output_connections_per_motif[i] = max_connect_per_motif;

        // not connections done for motifs yet
        n_input_connections_per_motif_done[i] = 0;
        n_output_connections_per_motif_done[i] = 0;
    }

    // TODO: This should be refactorized to a function
    // make some randomization to change the amount of input and output connections for motifs
    for(i = 0; i<ind->n_motifs; i++){
        n_change = rnd(0, n_input_connections_per_motif[i] - 1); // let at least one connection for the motif
        n_input_connections_per_motif[i] -= n_change;

        // select a motif (or a set of motifs) to give this input synapses
        while(n_change > 0){
            r_motif = rnd(0, ind->n_motifs-1); // motif to give connections
            n_con = rnd(1, n_change); // number of connections to give to this motif

            n_input_connections_per_motif[r_motif] += n_con;
            n_change -= n_con;
        }

        // Do the same with the output synapses
        n_change = rnd(0, n_output_connections_per_motif[i] - 1);
        n_output_connections_per_motif[i] -= n_change;

        // select a motif (or a set of motifs) to give this input synapses
        while(n_change > 0){
            r_motif = rnd(0, ind->n_motifs-1); // motif to give connections
            n_con = rnd(1, n_change); // number of connections to give to this motif

            n_output_connections_per_motif[r_motif] += n_con;
            n_change -= n_con;
        }
    }

    /* allocate memory for selected motifs */
    for(i = 0; i<ind->n_motifs; i++){
        selected_input_motifs_per_motif[i] = (int *)malloc(n_input_connections_per_motif[i] * sizeof(int));
        selected_output_motifs_per_motif[i] = (int *)malloc(n_output_connections_per_motif[i] * sizeof(int));
    }

    /* Connect the motifs */
    for(i = 0; i<ind->n_motifs; i++){
        //printf("i = %d\n", i);

        // select the motifs to connect with
        for(j = n_input_connections_per_motif_done[i]; j<n_input_connections_per_motif[i]; j++){
            //printf("j = %d\n", j);
            // select a random motif to connect with, and connect if that motif has connections remaining
            valid = 0; 
            while(valid == 0){
                tmp_motif = rnd(0, ind->n_motifs-1);

                // check if the selected motif has output connections free
                if(n_output_connections_per_motif[tmp_motif] > n_output_connections_per_motif_done[tmp_motif])
                    valid = 1;

                //printf("tmp_motif %d, n_output_done %d, n_output %d\n", tmp_motif, n_output_connections_per_motif_done[tmp_motif], n_output_connections_per_motif[tmp_motif]);
            }

            // add the connections
            selected_input_motifs_per_motif[i][j] = tmp_motif; // TODO: restrictions (some motif types can not be connected to other types...)    
            selected_output_motifs_per_motif[tmp_motif][n_output_connections_per_motif_done[tmp_motif]] = i;
    
            // sum output connection to the selected motif
            n_output_connections_per_motif_done[tmp_motif] ++;
        }
        n_input_connections_per_motif_done[i] = n_input_connections_per_motif[i];

        // select the motifs to connect with
        for(j = n_output_connections_per_motif_done[i]; j<n_output_connections_per_motif[i]; j++){
            //printf("j = %d\n", j);
            
            // select a random motif to connect with, and connect if that motif has connections remaining
            valid = 0; 
            while(valid == 0){
                tmp_motif = rnd(0, ind->n_motifs-1);

                // check if the selected motif has output connections free
                if(n_input_connections_per_motif_done[tmp_motif] < n_input_connections_per_motif[tmp_motif])
                    valid = 1;
            }

            // add the connections
            selected_output_motifs_per_motif[i][j] = tmp_motif; // TODO: restrictions (some motif types can not be connected to other types...)    
            selected_input_motifs_per_motif[tmp_motif][n_input_connections_per_motif_done[tmp_motif]] = i;

            // sum output connection to the selected motif
            n_input_connections_per_motif_done[tmp_motif] ++;            
        }
        n_output_connections_per_motif_done[i] = n_output_connections_per_motif[i];

        // order the lists (motifs from lower to higher)
        insertion_sort(selected_input_motifs_per_motif[i], n_input_connections_per_motif[i]);
        insertion_sort(selected_output_motifs_per_motif[i], n_output_connections_per_motif[i]);
    }


    // print information
    int sum_input_connections=0, sum_output_connections=0;
    printf("Printing number of input connections per motif: ");
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d, ", n_input_connections_per_motif[i]);
        sum_input_connections += n_input_connections_per_motif[i];
    }
    printf(" Total = %d\n", sum_input_connections);

    printf("Printing number of output connections per motif: ");
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d, ", n_output_connections_per_motif[i]);
        sum_output_connections += n_output_connections_per_motif[i];
    }
    printf(" Total = %d\n", sum_output_connections);


    // print connections
    printf("\n\nPrinting to which motifs each motif is connected: \n");
    for(i = 0; i<ind->n_motifs; i++){
        printf(" > Motif %d: [", i);
        for(j = 0; j<n_input_connections_per_motif[i]; j++){
            printf("%d, ", selected_input_motifs_per_motif[i][j]);
        }
        printf("][");
        for(j = 0; j<n_output_connections_per_motif[i]; j++){
            printf("%d, ", selected_output_motifs_per_motif[i][j]);
        }
        printf("]\n");
    }


    /* Construct the connectivity sparse matrix by a dynamic list */
    //construct_sparse_matrix(ind, n_connections_per_motif, selected_motifs_per_motif);
    printf("Starting to construct the sparse matrix\n");
    construct_sparse_matrix3(ind, n_input_connections_per_motif, selected_input_motifs_per_motif);

    /* free memory allocated by lists */
    for(i = 0; i<ind->n_motifs; i++){
        free(selected_input_motifs_per_motif[i]);
        free(selected_output_motifs_per_motif[i]);
    }
    free(selected_input_motifs_per_motif);
    free(selected_output_motifs_per_motif);

    free(n_input_connections_per_motif_done);
    free(n_output_connections_per_motif_done);

    free(n_input_connections_per_motif);
    free(n_output_connections_per_motif);
}

/* This function connects connects the input synapses to the first neurons */
void initialize_input_synapses(NSGA2Type *nsga2Params, individual *ind){
    int i;

    ind->n_input_synapses = nsga2Params->image_size; // TODO: Generalize to work not only with images
    sparse_matrix_node_t *input_synapse;

    // allocate memory for the first input synapse
    ind->input_synapses = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    // initialize the first synapse
    input_synapse = ind->input_synapses;

    // initialize the first synapse

    // initialize all synapses
    for(i = 0; i<ind->n_input_synapses; i++){
        input_synapse = initialize_sparse_matrix_node(ind, input_synapse, 1, -1, i); // it is only a column, so row -1, and the value is 1 always
    }
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
        printf("Initializing individual %d\n", i);
        initialize_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to initialize an individual randomly (memory is allocated here too)*/
void initialize_ind (NSGA2Type *nsga2Params, individual *ind)
{
    //OLD
    /*neuron_node_t *neuron;
    int neuron_index;

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = rnd(nsga2Params->min_motifs, nsga2Params->max_motifs); // random number of motifs
    
    // allocate memory to store individual motifs info
    ind->motifs = (motif_node_t *)malloc(ind->n_motifs * sizeof(motif_node_t)); // allocate memory to store motifs
    
    // initialize number of neurons and synapses of the individual to 0
    ind->n_neurons = 0;
    ind->n_input_neurons = 0;

    ind->n_synapses = 0;
    ind->n_input_synapses = 0;
    
    // initialize motifs
    initialize_ind_motifs(ind);

    // initialize neurons
    initialize_neuron_nodes(ind);

    // connect motifs and construct sparse matrix
    connect_motifs(ind);*/


    /* Initialize general parameters */

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = rnd(nsga2Params->min_motifs, nsga2Params->max_motifs);
    
    // initialize motifs
    initialize_ind_motifs(ind);
    printf("Motifs initialized!\n");

    // initialize neurons
    initialize_neuron_nodes(ind);
    printf("Neuron nodes initialized!\n");

    // Connect motifs and its neurons
    connect_motifs_and_neurons(ind);
    printf("Motifs and neuron connected!\n");

    // connect motifs and construct sparse matrix that indicates which neurons are connected
    connect_motifs(ind);
    printf("Motifs connected!\n");

    // initialize input synaptic connections
    initialize_input_synapses(nsga2Params, ind);
    
    return;
}
