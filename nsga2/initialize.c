/* Data initializtion routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h" 


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
        printf(" > Initializing individual %d\n", i);
        initialize_ind (nsga2Params, &(pop->ind[i]));
        printf("\n");
    }
    return;
}

/* Function to initialize an individual randomly (memory is allocated here too)*/
void initialize_ind (NSGA2Type *nsga2Params, individual *ind)
{
    // initialize general parameters
    general_initialization(nsga2Params, ind);
    printf(" >> General initializations done!\n");

    // initialize motifs
    initialize_ind_motifs(ind);
    printf(" >> Motifs initialized!\n");

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    set_neurons_behaviour(ind);
    printf(" >> Neuron nodes initialized!\n");

    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    printf(" >> Motifs and neuron connected!\n");

    // connect motifs and construct sparse matrix
    connect_motifs(ind);
    printf(" >> Motifs connected!\n");

    // initialize input synaptic connections (not included in the sparse matrix)
    initialize_input_synapses(nsga2Params, ind);
    printf(" >> Input synapses initialized!\n");

    return;
}

void general_initialization(NSGA2Type *nsga2Params, individual *ind){
    // initialize number of neurons and synapses of the individual to 0
    ind->n_neurons = 0;
    ind->n_input_neurons = nsga2Params->image_size; // this depends on the input // TODO: this approach is temporal

    ind->n_synapses = 0;
    ind->n_input_synapses = nsga2Params->image_size; // this depends on the input too // TODO: this approach is temporal

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = rnd(nsga2Params->min_motifs, nsga2Params->max_motifs); // min motifs and max motifs should depend on the input
}

/*
    Functions related to motifs initilization
*/

/* Function to initialize all motifs from a individual */
void initialize_ind_motifs(individual *ind){
    int i;
    int motif_type;
    new_motif_t *motif_node;

    // initialize dynamic list of motifs

    // allocate memory for the first motif
    ind->motifs_new = (new_motif_t *)malloc(sizeof(new_motif_t));
    motif_node = ind->motifs_new;

    // initialize first motif node
    initialize_motif_node(motif_node, 0, ind);

    // initialize the rest of motifs
    for(i = 1; i<ind->n_motifs; i++)
        motif_node = initialize_and_allocate_motif(motif_node, i, ind);
}

/* Function to allocate memory for new motifs and initialize them */
new_motif_t* initialize_and_allocate_motif(new_motif_t *motif_node, int motif_id, individual *ind){
        // allocate memory
        motif_node->next_motif = (new_motif_t *)malloc(sizeof(new_motif_t));
        motif_node = motif_node->next_motif; // move to the allocated motif 
        
        // initialize
        initialize_motif_node(motif_node, motif_id, ind);

        return motif_node;
}

/* Function to initialize a motif node */
void initialize_motif_node(new_motif_t *motif, int motif_id, individual *ind){
    // initialize motif data
    motif->motif_id = motif_id;
    motif->motif_type = rnd(0, n_motifs-1);
    motif->initial_global_index = ind->n_neurons;
    motif->next_motif = NULL; // next element is not initialized yet

    // add the amount of neurons of the motif to the individual
    ind->n_neurons += motifs_data[motif->motif_type].n_neurons;
}

/*
    Functions related to neurons initialization
*/

/* Function to initialize all neuron nodes from the neurons list */
void initialize_neuron_nodes(individual *ind){
    int i;
    neuron_node_t *neuron_node;

    // allocate memory for the first element of the neurons list
    ind->neurons = (neuron_node_t *)malloc(sizeof(neuron_node_t));

    // initialize first neuron
    neuron_node = ind->neurons;
    initialize_neuron_node(neuron_node);

    // loop to initialize the rest of the neurons
    for(i = 1; i<ind->n_neurons; i++)
        neuron_node = initialize_and_allocate_neuron_node(neuron_node);
}

neuron_node_t* initialize_and_allocate_neuron_node(neuron_node_t *neuron_node){
    // allocate memory for next neuron
    neuron_node->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t));
    neuron_node = neuron_node->next_neuron;

    // initialize neuron
    initialize_neuron_node(neuron_node);

    return neuron_node;
}

/* Function to initialize neuron nodes */
// TODO: this function should be more general and initialize neuron with some randomization
void initialize_neuron_node(neuron_node_t *neuron_node){
    neuron_node->threshold = 150;
    neuron_node->refract_time = 3;
    neuron_node->v_rest = 0;
    neuron_node->r = 10.0;
    neuron_node->next_neuron = NULL; // the next element is not initialized yet
}

void set_neurons_behaviour(individual *ind){
    int i;
    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    motif_node = ind->motifs_new;
    neuron_node = ind->neurons;

    while(motif_node != NULL){
        for(i = 0; i<motifs_data[motif_node->motif_type].n_neurons; i++){
            neuron_node->behaviour = motifs_data[motif_node->motif_type].neuron_behaviour[i];
            neuron_node = neuron_node->next_neuron;
        }
        motif_node = motif_node->next_motif;
    }
}


// TODO: TEST THIS FUNCTION
/* 
    Functions to connect motifs and neurons 
*/

/* Function to connect each motif with its first neuron in the neurons list */
void connect_motifs_and_neurons(individual *ind){
    int i,j;
    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    motif_node = ind->motifs_new;
    neuron_node = ind->neurons;

    // connect the first motif with the first neuron
    motif_node->first_neuron = neuron_node;

    // for each motif, find the first neuron using the number of neuron for each motif
    for(i = 1; i<ind->n_motifs; i++){
        motif_node = motif_node->next_motif;

        // advance the amount of neurons of the actual motif type
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++){
            neuron_node = neuron_node->next_neuron;
        }
        // set the neuron as the neuron of the motif
        motif_node->first_neuron = neuron_node;
    }
}


/*
    Functions to create synapses between motifs (neurons) and construct the sparse matrix
*/

/* Function to randomly selected to which motifs is each motif connected, and then build the sparse matrix */
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
    max_connect_per_motif = rnd(2, 5); // this is done like this to get a divisible number by // TODO: I think it should be dependent on the amount of motifs?
    printf(" >>> N motifs = %d, max connect per motif = %d\n", ind->n_motifs, max_connect_per_motif);

    /* Initialize the lists */
    for(i = 0; i<ind->n_motifs; i++){
        // set the amount of input and output connections for the motif 
        n_input_connections_per_motif[i] = max_connect_per_motif; // I am not very convinced with this
        n_output_connections_per_motif[i] = max_connect_per_motif;

        // not connections done for motifs yet
        n_input_connections_per_motif_done[i] = 0;
        n_output_connections_per_motif_done[i] = 0;
    }


    //===================================================================================================
    // TODO: This should be refactorized to a function: 
    // I think I could do all this only with input synapses if I ensure that all motifs are included

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

        // do the same with the output synapses
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

    /* connect the motifs */
    for(i = 0; i<ind->n_motifs; i++){
        // loop until all motifs to connect with are selected
        for(j = n_input_connections_per_motif_done[i]; j<n_input_connections_per_motif[i]; j++){
            // select a random motif to connect with, and connect if that motif has connections remaining
            valid = 0; 

            while(valid == 0){
                // select a motif randomly and check if it has any connection to be done yet
                tmp_motif = rnd(0, ind->n_motifs-1);

                // check if the selected motif has output connections free
                if(n_output_connections_per_motif[tmp_motif] > n_output_connections_per_motif_done[tmp_motif])
                    valid = 1;
            }

            // add the connections
            selected_input_motifs_per_motif[i][j] = tmp_motif; // TODO: restrictions (some motif types can not be connected to other types...)    
            selected_output_motifs_per_motif[tmp_motif][n_output_connections_per_motif_done[tmp_motif]] = i;
    
            // sum output connection to the selected motif
            n_output_connections_per_motif_done[tmp_motif] ++;
        }
        n_input_connections_per_motif_done[i] = n_input_connections_per_motif[i];

        // The same for output synapses
        for(j = n_output_connections_per_motif_done[i]; j<n_output_connections_per_motif[i]; j++){
            
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


    /* Print information about connections */
    // print information
    int sum_input_connections=0, sum_output_connections=0;
    printf(" >>> Printing number of input connections per motif: ");
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d, ", n_input_connections_per_motif[i]);
        sum_input_connections += n_input_connections_per_motif[i];
    }
    printf(" >>>> Total = %d\n", sum_input_connections);

    printf(" >>> Printing number of output connections per motif: ");
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d, ", n_output_connections_per_motif[i]);
        sum_output_connections += n_output_connections_per_motif[i];
    }
    printf(" >>>> Total = %d\n", sum_output_connections);

    // print connections
    printf(" >>> Printing to which motifs each motif is connected: \n");
    for(i = 0; i<ind->n_motifs; i++){
        printf(" >>>> Motif %d: [", i);
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

    printf(" >>> Starting to construct the sparse matrix...\n");
    construct_sparse_matrix(ind, n_input_connections_per_motif, selected_input_motifs_per_motif);
    printf(" >>> Sparse matrix constructed!\n");

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

/* Function to construct the sparse matrix of synaptic connections */
void construct_sparse_matrix(individual *ind, int *n_selected_input_motifs_per_motif, int **selected_input_motifs_per_motif){
    int i, j, k, s;
    int row, col, value;

    // The following two lists are auxiliary lists used to store motifs types and motifs first neurons global indexes as the dynamic list can not be
    // indexed
    int *motifs_types_list;
    int *motifs_neurons_global_index_list;

    // Other aux variables
    new_motif_t *motif_node, *tmp_motif_node;
    motif_t *motif_info, *tmp_motif_info;
    sparse_matrix_node_t *matrix_node, *tmp_first_matrix_node;
    int motif_type, first_neuron_global_index, neuron_index, tmp_motif_type, tmp_first_neuron_global_index, tmp_motif_index, tmp_neuron_index;
    int counter = 0;
    int actual_motif_processed = 0;


    /* Variables initialization */
    // get first motif node
    motif_node = ind->motifs_new;
    tmp_motif_node = motif_node;

    // initialize first sparse matrix node (memory not allocated)
    tmp_first_matrix_node = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t)); // this is only used 
    matrix_node = tmp_first_matrix_node;

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
    
    // set connectivity matrix list
    ind->connectivity_matrix = tmp_first_matrix_node->next_element;

    // free memory
    free(tmp_first_matrix_node);
    free(motifs_types_list);
    free(motifs_neurons_global_index_list);
}


// TODO: This function must be revised, changed, and tested
/* Function to add new synapses to an already constructed sparse matrix
    - ind: individual
    - n_new_motifs: number of new motifs added to the network
    - motif_node: the first new motif in the motif list
    - input_motif: selected input motifs for each new motif
    - output_motifs: selected output motifs for each new motif
*/
void construct_semi_sparse_matrix(individual *ind, int n_new_motifs, new_motif_t *motif_node, int_array_t *input_motifs, int_array_t *output_motifs){
    int i, j, k, s;
    int row, col, value;

    // The following two lists are auxiliary lists used to store motifs types and motifs first neurons global indexes as the dynamic list can not be
    // indexed
    int *motifs_types_list; // list to store the types of the new motifs that will be added
    int *motifs_neurons_global_index_list; // list to store the global indexes of the first neuron for each new motif

    // Other aux variables
    new_motif_t *tmp_motif_node;
    motif_t *motif_info, *tmp_motif_info;
    sparse_matrix_node_t *matrix_node;
    int motif_type, first_neuron_global_index, neuron_index, tmp_motif_type, tmp_first_neuron_global_index, tmp_motif_index, tmp_neuron_index;
    int counter = 0;
    int actual_motif_processed = 0;


    /* Variables initialization */
    tmp_motif_node = motif_node;

    // initialize first sparse matrix node
    matrix_node = ind->connectivity_matrix; // get the first element of the matrix

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
            for(k = 0; k<input_motifs[i].n; k++){
                // Get the information about this motif
                tmp_motif_index = input_motifs[i].array[k]; // get temp motif index in the motif list
                tmp_motif_type = motifs_types_list[tmp_motif_index];
                tmp_motif_info = &(motifs_data[tmp_motif_type]);
                tmp_first_neuron_global_index = motifs_neurons_global_index_list[tmp_motif_index];
                
                // count how much times this motif is connected to the one being computed
                counter = 1;
                while(k < input_motifs[i].n - 1 && 
                    input_motifs[i].array[k] == input_motifs[i].array[k+1]){ // if we are in the last it can no be repited
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

/* Function to add the nodes representing the motif internal structure to the dynamic list */
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

/* Function to allocate memory for the new sparse matrix node and initialize it */
sparse_matrix_node_t* initialize_sparse_matrix_node(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    int i;

    // allocate memory for the synapse
    matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = matrix_node->next_element; // move to the next synapse

    initialize_sparse_matrix_node_only(ind, matrix_node, value, row, col);

    return matrix_node;
}

/* Function to initialize a matrix node */
void initialize_sparse_matrix_node_only(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    int i;

    // initialize synapse
    matrix_node->value = value; // number of synaptic connections 
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

    matrix_node->next_element = NULL;

    // TODO: This should be in another place, but I don't know where
    // add amount of synapses to the individual number of synapses
    ind->n_synapses += abs(value);
}

/* Function to add new synapses and connect them to first neurons (input neurons) */
void initialize_input_synapses(NSGA2Type *nsga2Params, individual *ind){
    int i;

    sparse_matrix_node_t *input_synapse;

    // allocate memory for the first input synapse
    ind->input_synapses = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    // initialize the first synapse
    input_synapse = ind->input_synapses;
    initialize_sparse_matrix_node_only(ind, ind->input_synapses, 1, 0, 0); // it is only a column, so row -1, and the value is 1 always

    // initialize all synapses
    printf(" >>>>> n_input_neurons = %d\n", ind->n_input_neurons);
    printf(" >>>>> n_input_synapses = %d\n", ind->n_input_synapses);
    for(i = 1; i<ind->n_input_synapses; i++){
        // allocate memory for next input synapse and move to it
        //input_synapse->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
        //input_synapse = input_synapse->next_element;
        // initialize synapse
        printf(" >>>>> i = %d\n", i);
        input_synapse = initialize_sparse_matrix_node(ind, input_synapse, 1, 0, i); // it is only a column, so row -1, and the value is 1 always
    }
}

/* Function to initialize synapse weights */
void initialize_synapse_weights(NSGA2Type *nsga2Params, individual *ind){
    int i, j; 
    spiking_nn_t *snn = ind->snn;
    lif_neuron_t neuron;
    synapse_t *synapse;

    // input synapses are located at the begining of the list
    for(i=0; i<snn->n_input_synapses; i++){
        synapse = &(snn->synapses[i]);
        synapse->w = (double)rand() / RAND_MAX;

        // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution
        if(synapse->w < 100)
            synapse->w = 100;
        if(synapse->w > 500)
            synapse->w = 500;
    }

    // Initialize the rest of synapses
    for(i=0; i<snn->n_neurons; i++){
        // loop over neuron output synapses (weight sign depends on the behaviour of the neuron)
        neuron = snn->lif_neurons[i];
        for(j = 0; j<neuron.n_output_synapse; j++){
            synapse = &(snn->synapses[neuron.output_synapse_indexes[j]]);
            
            synapse->w = (double)rand() / RAND_MAX;

            // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution
            if(synapse->w < 10)
                synapse->w = 10;
            if(synapse->w > 200)
                synapse->w = 200;

            if(neuron.excitatory == -1){
                synapse->w = -synapse->w;
            }
        }
    }
}
