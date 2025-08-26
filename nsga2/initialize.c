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


/**
 * Functions to realise general initializations
 */

/* Function to initialize a population randomly */
void initialize_pop (NSGA2Type *nsga2Params, population *pop)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        #ifdef DEBUG2
        printf(" > Initializing individual %d\n", i);
        #endif

        initialize_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to initialize an individual */
void initialize_ind (NSGA2Type *nsga2Params, individual *ind)
{
    // initialize general parameters (number of motifs, neurons...)
    general_initialization(nsga2Params, ind);

    // initialize motifs (dynamic list of motifs)
    initialize_ind_motifs(ind);

    // initialize neurons (dynamic list of neurons)
    initialize_neuron_nodes(nsga2Params, ind);

    // set the behaviour for neurons (excitatory or inhibitory)
    set_neurons_behaviour(ind);

    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);

    // select to which motifs each motif is connected and build the sparse matrix that contains the information of the synaptic connections
    connect_motifs(nsga2Params, ind);

    // initialize input synaptic connections (not included in the sparse matrix)
    initialize_input_synapses(nsga2Params, ind);


    // in phase 1 learning zones are not used, so initialize only in second phase
    ind->learning_zones = NULL;

#ifdef PHASE2
    initialize_learning_zones_individual(nsga2Params, ind);    
#endif


    // check initialization
    new_motif_t *motif_node = ind->motifs_new;
    int counter = 0;
    while(motif_node){
        counter ++;
        motif_node = motif_node->next_motif;
    }

    printf(" > > n_motifs = %d, counter = %d\n", ind->n_motifs, counter);
    fflush(stdout);

    neuron_node_t *neuron_node = ind->neurons;
    counter = 0;
    while(neuron_node){
        counter ++;
        neuron_node = neuron_node->next_neuron;
    }

    printf(" > > n_neurons = %d, counter = %d\n", ind->n_neurons, counter);
    fflush(stdout);


    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;
    counter = 0;
    while(synapse_node){
        counter += abs(synapse_node->value);
        synapse_node = synapse_node->next_element;
    }

    printf(" > > n_synapses = %d, counter = %d\n", ind->n_synapses - ind->n_input_synapses, counter);
    fflush(stdout);

    

    return;
}

/* Function to initialize an individual without randomization */
void initialize_ind_not_random (NSGA2Type *nsga2Params, individual *ind, int n_motifs, int *motif_types, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs)
{
    // initialize general parameters (number of motifs, neurons...)
    general_initialization_not_random_motifs(nsga2Params, ind, n_motifs);
    printf(" General initializations done\n");
    fflush(stdout);

    // initialize motifs (dynamic list of motifs)
    initialize_ind_motifs_from_types(ind, motif_types);
    printf(" Motifs initialized\n");
    fflush(stdout);

    // initialize neurons (dynamic list of neurons)
    initialize_neuron_nodes(nsga2Params, ind);
    printf(" Neuron initialized\n");
    fflush(stdout);

    // set the behaviour for neurons (excitatory or inhibitory)
    set_neurons_behaviour(ind);
    printf(" Neurons behaviours initialized\n");
    fflush(stdout);

    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    printf(" Motifs connected to neurons\n");
    fflush(stdout);

    // select to which motifs each motif is connected and build the sparse matrix that contains the information of the synaptic connections
    connect_motifs_from_arrays(nsga2Params, ind, selected_input_motifs, selected_output_motifs);
    printf(" Motifs connected!\n");
    fflush(stdout);

    // initialize input synaptic connections (not included in the sparse matrix)
    initialize_input_synapses(nsga2Params, ind);
    printf(" Input synapses initialzied\n");
    fflush(stdout);

    return;
}

/* Function to realise general initializations for an individual */
void general_initialization(NSGA2Type *nsga2Params, individual *ind){
    // initialize number of neurons    
    ind->n_neurons = 0;
    ind->n_input_neurons = nsga2Params->image_size; // this depends on the input // TODO: this approach is temporal

    // initialize number of synapses
    ind->n_synapses = 0;
    ind->n_input_synapses = nsga2Params->image_size; // this depends on the input too // TODO: this approach is temporal

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = rnd(nsga2Params->min_motifs, nsga2Params->max_motifs);
}

/* Function to realise general initializations for an individual WITHOUT randomization (number of motifs is provided) */
void general_initialization_not_random_motifs(NSGA2Type *nsga2Params, individual *ind, int n_motifs){
    // initialize number of neurons
    ind->n_neurons = 0;
    ind->n_input_neurons = nsga2Params->image_size; // this depends on the input // TODO: this approach is temporal

    // initialize number of synapses
    ind->n_synapses = 0;
    ind->n_input_synapses = nsga2Params->image_size; // this depends on the input too // TODO: this approach is temporal

    // initialize randomly the amount of motifs for this individual
    ind->n_motifs = n_motifs;
}


/**
 * Functions related to motifs initialization
 */

/* Function to initialize all motifs of an individual */
void initialize_ind_motifs(individual *ind){
    int i;
    new_motif_t *motif_node;

    // initialize dynamic list of motifs

    // allocate memory for the first motif
    ind->motifs_new = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    motif_node = ind->motifs_new;

    // initialize first motif node
    initialize_motif_node(motif_node, 0, ind);

    // initialize the rest of motifs
    for(i = 1; i<ind->n_motifs; i++)
        motif_node = initialize_and_allocate_motif(motif_node, i, ind);

    // print motif types
    #ifdef DEBUG3
        // print motif types
        printf(" > > Motif types: ");
        motif_node = ind->motifs_new;

        while(motif_node != NULL){
            printf("%d ", motif_node->motif_type);
            motif_node = motif_node->next_motif;
        }

        printf("\n");
    #endif
}

/* Function to initilaize the motifs of a network given the motif types */
void initialize_ind_motifs_from_types(individual *ind, int *motif_types){
    int i;
    new_motif_t *motif_node;

    // initialize dynamic list of motifs

    // allocate memory for the first motif
    ind->motifs_new = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    motif_node = ind->motifs_new;

    // initialize first motif node
    initialize_motif_node_from_type(motif_node, 0, ind, motif_types[0]);

    // initialize the rest of motifs
    for(i = 1; i<ind->n_motifs; i++)
        motif_node = initialize_and_allocate_motif_from_type(motif_node, i, ind, motif_types[i]);
}

/* Function to allocate memory for a motif node and initialize it */
new_motif_t* initialize_and_allocate_motif(new_motif_t *motif_node, int motif_id, individual *ind){
    // allocate memory
    motif_node->next_motif = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    motif_node = motif_node->next_motif; // move to the allocated motif 
    
    // initialize
    initialize_motif_node(motif_node, motif_id, ind);

    return motif_node;
}

/* Function to initialize a motif node with some randomization */
void initialize_motif_node(new_motif_t *motif, int motif_id, individual *ind){
    // initialize motif data
    motif->motif_id = motif_id;
    motif->motif_type = rnd(0, n_motifs-1);
    motif->initial_global_index = ind->n_neurons;
    motif->next_motif = NULL; // next element is not initialized yet
    motif->in_lz = 0; // motif not added yet to learning zone
    motif->lz_index = -1;

    // add the amount of neurons of the motif to the individual
    ind->n_neurons += motifs_data[motif->motif_type].n_neurons;
}

/* Function to allocate memory for a motif node and initialize it given the motif type */
new_motif_t* initialize_and_allocate_motif_from_type(new_motif_t *motif_node, int motif_id, individual *ind, int type){
        // allocate memory
        motif_node->next_motif = (new_motif_t *)calloc(1, sizeof(new_motif_t));
        motif_node = motif_node->next_motif; // move to the allocated motif 
        
        // initialize
        initialize_motif_node_from_type(motif_node, motif_id, ind, type);

        return motif_node;
}

/* Function to initialize a motif node given the motif type */
void initialize_motif_node_from_type(new_motif_t *motif, int motif_id, individual *ind, int type){
    // initialize motif data
    motif->motif_id = motif_id;
    motif->motif_type = type;
    motif->initial_global_index = ind->n_neurons;
    motif->next_motif = NULL; // next element is not initialized yet

    // add the amount of neurons of the motif to the individual
    ind->n_neurons += motifs_data[motif->motif_type].n_neurons;
}


/**
 * Functions related to neuron nodes
 */

/* Function to initialize all neuron nodes of the neurons list */
void initialize_neuron_nodes(NSGA2Type *nsga2Params, individual *ind){
    int i;
    neuron_node_t *neuron_node;

    // allocate memory for the first element of the neurons list
    ind->neurons = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));

    // initialize first neuron
    neuron_node = ind->neurons;
    initialize_neuron_node(nsga2Params, neuron_node);

    // loop to initialize the rest of the neurons
    for(i = 1; i<ind->n_neurons; i++)
        neuron_node = initialize_and_allocate_neuron_node(nsga2Params, neuron_node);
}

/* Function to allocate memory for the next neuron node and initialize it */
neuron_node_t* initialize_and_allocate_neuron_node(NSGA2Type *nsga2Params, neuron_node_t *neuron_node){
    // allocate memory for next neuron
    neuron_node->next_neuron = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
    neuron_node = neuron_node->next_neuron;

    // initialize neuron
    initialize_neuron_node(nsga2Params, neuron_node);

    return neuron_node;
}

/* Function to initialize a neuron node (with some randomization) */
void initialize_neuron_node(NSGA2Type *nsga2Params, neuron_node_t *neuron_node){
    neuron_node->threshold = rndreal(nsga2Params->min_vthres, nsga2Params->max_vthres);
    neuron_node->refract_time = rnd(nsga2Params->min_refracttime, nsga2Params->max_refracttime);
    neuron_node->v_rest = rndreal(nsga2Params->min_vrest, nsga2Params->max_vrest);
    neuron_node->r = rnd(nsga2Params->min_R, nsga2Params->max_R);

    neuron_node->next_neuron = NULL; // the next element is not initialized yet
}

/* Function to allocate memory for the next neuron node and initialize it, but indicating the behaviour (excitatory or inhibitory) */
neuron_node_t* initialize_and_allocate_neuron_node_and_behaviour(NSGA2Type *nsga2Params, neuron_node_t *neuron_node, int behav){
    // allocate memory for next neuron
    neuron_node->next_neuron = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
    neuron_node = neuron_node->next_neuron;

    // initialize neuron
    initialize_neuron_node_and_behaviour(nsga2Params, neuron_node, behav);

    return neuron_node;
}

/* Function to initialize a neuron node given the behaviour (excitatory or inhibitory) */
void initialize_neuron_node_and_behaviour(NSGA2Type *nsga2Params, neuron_node_t *neuron_node, int behav){
    neuron_node->threshold = rndreal(nsga2Params->min_vthres, nsga2Params->max_vthres);
    neuron_node->refract_time = rnd(nsga2Params->min_refracttime, nsga2Params->max_refracttime);
    neuron_node->v_rest = rndreal(nsga2Params->min_vrest, nsga2Params->max_vrest);
    neuron_node->r = rnd(nsga2Params->min_R, nsga2Params->max_R);
    neuron_node->behaviour = behav;

    neuron_node->next_neuron = NULL; // the next element is not initialized yet
}


/* Function to set the behaviour (excitatory or inhibitory) of all the neurons in the individual */
void set_neurons_behaviour(individual *ind){
    int i;
    motif_t *motif_data;

    // get first motif and first neuron nodes
    new_motif_t *motif_node = ind->motifs_new;
    neuron_node_t *neuron_node = ind->neurons;

    // motif info is used to set the behaviour
    while(motif_node){
        // get the information about this motif type
        motif_data = &(motifs_data[motif_node->motif_type]);

        // loop over motif neurons to set the behaviour
        for(i = 0; i<motif_data->n_neurons; i++){
            neuron_node->behaviour = motif_data->neuron_behaviour[i]; // set behaviour
            neuron_node = neuron_node->next_neuron; // move to the next neuron node
        }
        // move to the next motif
        motif_node = motif_node->next_motif;
    }
}


/* 
    Functions to connect motifs and neurons 
*/

/**
 * Function to connect motifs with their first neuron in the dynamic lists
 */
/* Function to connect each motif with its first neuron in the neurons list (this can be used in any moment to reconnect) */
void connect_motifs_and_neurons(individual *ind){
    int i,j;

    // get the first motif and neuron nodes
    new_motif_t *motif_node = ind->motifs_new;
    neuron_node_t *neuron_node = ind->neurons;

    // connect the first motif with the first neuron
    motif_node->first_neuron = neuron_node;

    // loop over motifs and neurons to connect each motif with its first neuron in the neuron list
    for(i = 1; i<ind->n_motifs; i++){
        // move to the next motif
        motif_node = motif_node->next_motif;

        // advance the amount of neurons of the actual motif type
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++){
            neuron_node = neuron_node->next_neuron;
        }
        
        // set the neuron as the neuron of the motif
        motif_node->first_neuron = neuron_node;
    }
}

/**
 * Functions to connect the motifs of an individual
 */

/* Function to randomly selected to which motifs is each motif connected, and then build the sparse matrix */
void connect_motifs(NSGA2Type *nsga2Params, individual *ind){
    
    int i, j, tmp_motif, valid, max_connect_per_motif, min_val, max_val;
    new_motif_t *motif;
    int_node_t *int_node;

    /* allocate memory for lists */
    int *n_input_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int)); // number of input connections between motifs per each motif
    int *n_output_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int)); // number of output connections of the actual motif with other motifs

    int *n_input_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int)); // number of input connections between motifs per each motif already done
    int *n_output_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int)); // number of output connections of the actual motif with other motifs already done

    int_array_t *selected_input_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));
    int_array_t *selected_output_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));


    // set the number of input and output motifs for each motif in the individual
    min_val = (int)(ind->n_motifs * nsga2Params->min_percentage_connectivity);
    max_val = (int)(ind->n_motifs * nsga2Params->max_percentage_connectivity);
    //min_val = 2;
    //max_val = 5;
    max_connect_per_motif = rnd(min_val, max_val); // this is the maximum number of connections that a motif can contain as input or output //rnd(2, 5);
    set_number_of_connections(ind, n_input_connections_per_motif, n_input_connections_per_motif_done, max_connect_per_motif);
    set_number_of_connections(ind, n_output_connections_per_motif, n_output_connections_per_motif_done, max_connect_per_motif);


    // select input and output motifs for each motif
    for(i = 0; i<ind->n_motifs; i++){
        selected_input_motifs[i].array = (int *)calloc(n_input_connections_per_motif[i], sizeof(int));
        selected_input_motifs[i].n = n_input_connections_per_motif[i];
        selected_output_motifs[i].array = (int *)calloc(n_output_connections_per_motif[i], sizeof(int));
        selected_output_motifs[i].n = n_output_connections_per_motif[i];
    }
    select_input_and_output_motifs_per_motif(ind, selected_input_motifs, selected_output_motifs, 
                                                n_input_connections_per_motif_done, n_output_connections_per_motif_done);


    // store the information of the input and output motifs for each motifs in the individual struct
    initialize_lists_of_connectivity(ind, selected_input_motifs, selected_output_motifs);


    // print information
    #ifdef DEBUG3
        printf(" > > > Input motifs for each motif: \n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > Motif %d: ", i);
            for(j = 0; j<selected_input_motifs[i].n; j++){
                printf("%d ", selected_input_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);

        printf(" > > > Output motifs for each motif: \n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > Motif %d: ", i);
            for(j = 0; j<selected_output_motifs[i].n; j++){
                printf("%d ", selected_output_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);

            #ifdef DEBUG3
        printf(" > > > > > Printing input motifs from dynamic list:\n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
            if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }

        printf(" > > > > > Printing output motifs from dynamic list:");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
            if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }

        printf("\n > > > > Updating dynamic lists...\n");
        fflush(stdout);

    #endif
    #endif

    // build the sparse matrix of synapses
    build_sparse_matrix(nsga2Params, ind, selected_input_motifs);


    /* free memory allocated by lists */
    deallocate_int_arrays(selected_input_motifs, ind->n_motifs); // not only the global structure is deallocated, but all the arrays inside too
    deallocate_int_arrays(selected_output_motifs, ind->n_motifs);
    
    free(n_input_connections_per_motif_done);
    free(n_output_connections_per_motif_done);

    free(n_input_connections_per_motif);
    free(n_output_connections_per_motif);
}

/* Function to connect motifs and initialize structs to store the information */
void connect_motifs_from_arrays(NSGA2Type *nsga2Params, individual *ind, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs){
    
    int i, j, tmp_motif, valid, max_connect_per_motif, min_val, max_val;
    new_motif_t *motif;
    int_node_t *int_node;

    // store the information of the input and output motifs for each motifs in the individual struct
    initialize_lists_of_connectivity(ind, selected_input_motifs, selected_output_motifs);
    //printf(" Connectivity initialized\n");
    fflush(stdout);

    // print information
    #ifdef DEBUG3
        printf(" > > > Input motifs for each motif: \n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > Motif %d: ", i);
            for(j = 0; j<selected_input_motifs[i].n; j++){
                printf("%d ", selected_input_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);

        printf(" > > > Output motifs for each motif: \n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > Motif %d: ", i);
            for(j = 0; j<selected_output_motifs[i].n; j++){
                printf("%d ", selected_output_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);

            #ifdef DEBUG3
        printf(" > > > > > Printing input motifs from dynamic list:\n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
            if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }

        printf(" > > > > > Printing output motifs from dynamic list:");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
            if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }

        printf("\n > > > > Updating dynamic lists...\n");
        fflush(stdout);

    #endif
    #endif

    // build the sparse matrix of synapses
    build_sparse_matrix(nsga2Params, ind, selected_input_motifs);
    //printf(" Sparse matrix built\n");
    //fflush(stdout);
}

/* Function to set to how much input or output motifs is connected each motif */
void set_number_of_connections(individual *ind, int *n_connection_per_motif, int *n_connection_per_motif_done, int max_connect_per_motif){
    int i;

    // initialize input and output motifs list (max size max_connect_per_motif)
    for(i = 0; i<ind->n_motifs; i++){
        // set the amount of input and output connections for the motif 
        n_connection_per_motif[i] = max_connect_per_motif; // I am not very convinced with this
        n_connection_per_motif[i] = max_connect_per_motif;

        // not connections done yet
        n_connection_per_motif_done[i] = 0;
        n_connection_per_motif_done[i] = 0;
    }

    // randomize number of connections for each motif
    randomize_motif_connections(ind, n_connection_per_motif);
}

/* Function to selected to which motifs is connected is motif */
void randomize_motif_connections(individual *ind, int *n_connections_per_motif){
    int i, n_change, tmp_motif, n_con;
    
    for(i = 0; i<ind->n_motifs; i++){
        n_change = rnd(0, n_connections_per_motif[i] - 1); // let at least one connection for the motif
        n_connections_per_motif[i] -= n_change; // we are moving n_change connections to another motif

        // select a motif (or a set of motifs) to give these connections
        while(n_change > 0){
            tmp_motif = rnd(0, ind->n_motifs-1); // motif to give connections
            n_con = rnd(1, n_change); // number of motif connections to give to this motif

            n_connections_per_motif[tmp_motif] += n_con; // add n_con connections to the motif
            n_change -= n_con; // now we have less connections to move
        }
    }
}

/* Function to select input or output motifs connected to each motif */
void select_input_and_output_motifs_per_motif(individual *ind, int_array_t *selected_input_motifs_per_motif, int_array_t *selected_output_motifs_per_motif,  
                                            int *n_input_connections_per_motif_done, int *n_output_connections_per_motif_done){
    
    int i, j, valid, tmp_motif;

    for(i = 0; i<ind->n_motifs; i++){

        // loop until all motifs to connect with are selected
        for(j = n_input_connections_per_motif_done[i]; j<selected_input_motifs_per_motif[i].n; j++){
            // select a random motif to connect with, and connect if that motif has connections remaining
            valid = 0; 

            while(valid == 0){
                // select a motif randomly and check if it has any connection to be done yet
                tmp_motif = rnd(0, ind->n_motifs-1); // TODO: this can be very slow in big networks

                // check if the selected motif has output connections free
                if(selected_output_motifs_per_motif[tmp_motif].n > n_output_connections_per_motif_done[tmp_motif])
                    valid = 1;
            }

            // add the motif as input and output for the corresponding motifs
            selected_input_motifs_per_motif[i].array[j] = tmp_motif; // TODO: restrictions (some motif types can not be connected to other types...)    
            selected_output_motifs_per_motif[tmp_motif].array[n_output_connections_per_motif_done[tmp_motif]] = i;
    
            // sum output connection to the selected motif
            n_output_connections_per_motif_done[tmp_motif] ++;
        }
        n_input_connections_per_motif_done[i] = selected_input_motifs_per_motif[i].n;

        // The same for output synapses
        for(j = n_output_connections_per_motif_done[i]; j<selected_output_motifs_per_motif[i].n; j++){
            
            // select a random motif to connect with, and connect if that motif has connections remaining
            valid = 0; 
            while(valid == 0){
                tmp_motif = rnd(0, ind->n_motifs-1);

                // check if the selected motif has output connections free
                if(n_input_connections_per_motif_done[tmp_motif] < selected_input_motifs_per_motif[tmp_motif].n)
                    valid = 1;
            }

            // add the connections
            selected_output_motifs_per_motif[i].array[j] = tmp_motif; // TODO: restrictions (some motif types can not be connected to other types...)    
            selected_input_motifs_per_motif[tmp_motif].array[n_input_connections_per_motif_done[tmp_motif]] = i;

            // sum output connection to the selected motif
            n_input_connections_per_motif_done[tmp_motif] ++;            
        }
        n_output_connections_per_motif_done[i] = selected_output_motifs_per_motif[i].n;

        // order the lists (motifs from lower to higher)
        insertion_sort(selected_input_motifs_per_motif[i].array, selected_input_motifs_per_motif[i].n);
        insertion_sort(selected_output_motifs_per_motif[i].array, selected_output_motifs_per_motif[i].n);
    }
}

void initialize_lists_of_connectivity(individual *ind, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs){
    
    int i, j;
    int_node_t *int_node;

    // use the information of the selected input and output motifs for each motif to initialize the dynamic lists that will store this information
                                                
    // allocate memory for dynamic lists to store motif connectivity information. Larger lists than neccessary are allocated as network will raise
    ind->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind->connectivity_info.n_max_motifs = ind->n_motifs * 10; 

    // loop and initialize all int nodes
    for(i = 0; i<ind->n_motifs; i++){

        // copy number of input and output nodes (motifs) for each motif
        ind->connectivity_info.in_connections[i].n_nodes = selected_input_motifs[i].n;
        ind->connectivity_info.out_connections[i].n_nodes = selected_output_motifs[i].n;

        // if number of input connections is bigger than 0, then initialize dynamic list
        if(ind->connectivity_info.in_connections[i].n_nodes > 0){

            // initialize first element            
            int_node = initialize_and_allocate_int_node(selected_input_motifs[i].array[0], NULL, NULL); // get first node of the list
            ind->connectivity_info.in_connections[i].first_node = int_node; // connect int node to the dynamic list

            // initialize the rest of nodes
            for(j = 1; j<ind->connectivity_info.in_connections[i].n_nodes; j++)
                int_node = initialize_and_allocate_int_node(selected_input_motifs[i].array[j], int_node, NULL); // int node is the previous for tbe new one
        }

        // the same for the output motifs
        if(ind->connectivity_info.out_connections[i].n_nodes > 0){

            // initialize first element            
            int_node = initialize_and_allocate_int_node(selected_output_motifs[i].array[0], NULL, NULL);
            ind->connectivity_info.out_connections[i].first_node = int_node;

            // initialize the rest of nodes
            for(j = 1; j<ind->connectivity_info.out_connections[i].n_nodes; j++)
                int_node = initialize_and_allocate_int_node(selected_output_motifs[i].array[j], int_node, NULL);
        }
    }
}

/* Function to build the sparse matrix of synaptic connections */
void build_sparse_matrix(NSGA2Type *nsga2Params, individual *ind, int_array_t *selected_input_motifs){

    int i;

    new_motif_t *motif_node;
    sparse_matrix_node_t *synapse_node, *first_synapse_node;
    sparse_matrix_build_info_t SMBI;

    // allocate memory for aux lists and initialize
    SMBI.motifs_types = (int *)malloc(ind->n_motifs * sizeof(int));
    SMBI.motifs_first_neuron_indexes = (int *)malloc(ind->n_motifs * sizeof(int));
    SMBI.actual_motif_proccessed = 0;

    // get motifs general information
    motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        SMBI.motifs_types[i] = motif_node->motif_type; // store the motif type
        SMBI.motifs_first_neuron_indexes[i] = motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        motif_node = motif_node->next_motif;
    }

    // initialize first sparse matrix node (memory not allocated)
    first_synapse_node = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t)); // this is only used to store the pointer of the first synapse node of the matrix, then, the first is ignored as the real matrix starts in the next elemnet
    synapse_node = first_synapse_node;

    // loop over motifs to construct motif columns iteratively
    motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        // store actual motif information
        SMBI.actual_motif_index = i;
        SMBI.actual_motif_type = SMBI.motifs_types[i];
        SMBI.actual_motif_first_neuron_index = SMBI.motifs_first_neuron_indexes[i];
        SMBI.actual_motif_info = &(motifs_data[SMBI.actual_motif_type]);
        
        // construct sparse matrix
        synapse_node = build_motif_sparse_matrix_columns(nsga2Params, ind, synapse_node, &(selected_input_motifs[i]), &SMBI);

        // move to the next motif
        motif_node->next_motif;
    }

    // connect the individual to the matrix first node
    ind->connectivity_matrix = first_synapse_node->next_element;

    // set NULL after the last synapse of the sparse matrix
    synapse_node->next_element = NULL;

    // free allocated memory
    free(first_synapse_node);
    free(SMBI.motifs_types);
    free(SMBI.motifs_first_neuron_indexes);
}

/* Function to build the columns of a motif in the sparse matrix of synapses */
sparse_matrix_node_t* build_motif_sparse_matrix_columns(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI){
    
    // loop over motif neurons
    for(int i=0; i<SMBI->actual_motif_info->n_neurons; i++){
        SMBI->actual_neuron_local_index = i;
        synapse_node = build_neuron_sparse_matrix_column(nsga2Params, ind, synapse_node, selected_input_motifs, SMBI);
    }
    return synapse_node;
}

/* Function to build a column of a motif in the sparse matrix of synapses */
sparse_matrix_node_t* build_neuron_sparse_matrix_column(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI){
    
    int i;

    // loop over actual motif input motifs to construct the cells
    for(i=0; i<selected_input_motifs->n; i++){

        if(i == 0)
            SMBI->previous_input_motif_index = -1;
        else
            SMBI->previous_input_motif_index = selected_input_motifs->array[i-1];

        // get and store input motif info
        SMBI->input_motif_index = selected_input_motifs->array[i];
        SMBI->input_motif_type = SMBI->motifs_types[SMBI->input_motif_index];
        SMBI->input_motif_first_neuron_index = SMBI->motifs_first_neuron_indexes[SMBI->input_motif_index];
        SMBI->input_motif_info = &(motifs_data[SMBI->input_motif_type]);


        // count how much times the input motif appears in the list of input motifs
        SMBI->n_connections = 1;
        while(i < selected_input_motifs->n - 1 && selected_input_motifs->array[i] == selected_input_motifs->array[i+1]){ // if we are in the last it can no be repited
            SMBI->n_connections ++; // there is a repetition
            i++; // move forward in i index
        }

        // get the next input motif index. It the actual one is the last, set to -1
        if(i + 1 == selected_input_motifs->n)
            SMBI->next_input_motif_index = -1;
        else
            SMBI->next_input_motif_index = selected_input_motifs->array[i+1];

        synapse_node = add_neuron_to_motif_connections(nsga2Params, ind, synapse_node, SMBI);
    }
    return synapse_node;
}

/* Function to add the "cell" of a neuron (motif neuron connection) in the sparse matrix of synapses */
sparse_matrix_node_t* add_neuron_to_motif_connections(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
    int i, j;
    int actual_motif_index, input_motif_index, previous_input_motif_index, next_input_motif_index;

    actual_motif_index = SMBI->actual_motif_index;
    input_motif_index = SMBI->input_motif_index;
    previous_input_motif_index = SMBI->previous_input_motif_index;
    next_input_motif_index = SMBI->next_input_motif_index;

    // Check different cases to construct the structure

    if (input_motif_index == actual_motif_index) {
        // loop over motif neurons
        synapse_node = build_motif_internal_structure_and_connection(nsga2Params, ind, synapse_node, SMBI);
    }
    else if (input_motif_index < actual_motif_index) {
        // loop over motif neurons
        synapse_node = build_connection(nsga2Params, ind, synapse_node, SMBI);
        
        if(next_input_motif_index == -1){
            synapse_node = build_motif_internal_structure(nsga2Params, ind, synapse_node, SMBI);
        }
    }
    else {// (input_motif_index > actual_motif_index) {
        // loop over motif neurons
        if(previous_input_motif_index < actual_motif_index && SMBI->actual_motif_proccessed == 0){ 
            synapse_node = build_motif_internal_structure(nsga2Params, ind, synapse_node, SMBI);
        }

        synapse_node = build_connection(nsga2Params, ind, synapse_node, SMBI);
    }
    return synapse_node;
}

/* Function to build the internal structure of a motif */
sparse_matrix_node_t* build_motif_internal_structure(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    int i, row, col, value, actual_motif_first_neuron_index, actual_neuron_local_index;
    motif_t *actual_motif_info;

    // get the information of the actual motif
    actual_motif_info = SMBI->actual_motif_info;
    actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    actual_neuron_local_index = SMBI->actual_neuron_local_index;

    // loop over the neuron_index.th neuron column to find the input neurons in the internal structure of the motif
    for(i = 0; i<actual_motif_info->n_neurons; i++){
        
        // check if there is an input neuron in that position 
        if(actual_motif_info->connectivity_matrix[i * actual_motif_info->n_neurons + actual_neuron_local_index] != 0){ // there is a connection at least
            row = actual_motif_first_neuron_index + i;
            col = actual_motif_first_neuron_index + actual_neuron_local_index;
            value = actual_motif_info->connectivity_matrix[i * actual_motif_info->n_neurons + actual_neuron_local_index];

            synapse_node = initialize_sparse_matrix_node(nsga2Params, ind, synapse_node, value, row, col);
        }
    }
    return synapse_node;
}

/* Function to build the connection of a neuron with a motif */
sparse_matrix_node_t* build_connection(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
    int i, row, col, value;
    int actual_motif_type, input_motif_type, actual_motif_first_neuron_index, actual_neuron_local_index, input_motif_first_neuron_index, n_connections;
    motif_t *actual_motif_info, *input_motif_info;

    // get input motif information
    actual_motif_info = SMBI->actual_motif_info;
    input_motif_info = SMBI->input_motif_info;

    actual_motif_type = SMBI->actual_motif_type;
    input_motif_type = SMBI->input_motif_type;

    actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    input_motif_first_neuron_index = SMBI->input_motif_first_neuron_index;

    actual_neuron_local_index = SMBI->actual_neuron_local_index;
    n_connections = SMBI->n_connections;

    // check if the actual neuron is an input neuron, if not, it is not neccessary to add new connections
    if(check_if_neuron_is_input(actual_motif_type, actual_neuron_local_index) == 1){
        
        // loop over input motif neurons and check if they are output neurons to connect with the actual motif neuron
        for(i = 0; i<input_motif_info->n_neurons; i++){
            if(check_if_neuron_is_output(input_motif_type, i) == 1){
                value = n_connections;
                row = input_motif_first_neuron_index + i; // we are processing the i.th neuron of the input motif
                col = actual_motif_first_neuron_index + actual_neuron_local_index; // we are processing the neuron_local_index.th neuron of the actual motif

                synapse_node = initialize_sparse_matrix_node(nsga2Params, ind, synapse_node, value, row, col);
            }
        }
    }
    return synapse_node;
}

/* Function to build  */
sparse_matrix_node_t* build_motif_internal_structure_and_connection(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
    int i, row, col, value;
    int actual_motif_type, input_motif_type, actual_motif_first_neuron_index, actual_neuron_local_index, input_motif_first_neuron_index, n_connections;
    motif_t *actual_motif_info, *input_motif_info;

    // get input motif information
    actual_motif_info = SMBI->actual_motif_info;
    input_motif_info = SMBI->input_motif_info;

    actual_motif_type = SMBI->actual_motif_type;
    input_motif_type = SMBI->input_motif_type;

    actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    input_motif_first_neuron_index = SMBI->input_motif_first_neuron_index;

    actual_neuron_local_index = SMBI->actual_neuron_local_index;
    n_connections = SMBI->n_connections;
    
    // process the motif internal structure and the connections between motifs at the same time (motif connected with itself)
    // loop over the actual neuron column looking for connections
    for(i = 0; i<actual_motif_info->n_neurons; i++){
        value = actual_motif_info->connectivity_matrix[i * actual_motif_info->n_neurons + actual_neuron_local_index]; // check if there is a connection in the internal structure

        // check if the actual one is the input neuron and the other an output neurons
        if(check_if_neuron_is_input(actual_motif_type, actual_neuron_local_index) == 1 && check_if_neuron_is_output(input_motif_type, i) == 1){
            if(value < 0) value -= n_connections;
            else if (value > 0) value += n_connections;
            else value = n_connections;
        }

        // if there is any connection, add to the list
        if(value != 0){
            row = actual_motif_first_neuron_index + i;
            col = actual_motif_first_neuron_index + actual_neuron_local_index;

            synapse_node = initialize_sparse_matrix_node(nsga2Params, ind, synapse_node, value, row, col);
        }
    }
    return synapse_node;
}

/* Function to allocate memory for the new sparse matrix node and initialize it */
sparse_matrix_node_t* initialize_sparse_matrix_node(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    // allocate memory for the synapse
    matrix_node->next_element = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    matrix_node = matrix_node->next_element; // move to the next synapse

    initialize_sparse_matrix_node_only(nsga2Params, ind, matrix_node, value, row, col);

    return matrix_node;
}

int exp_distribution(double lambda){
    double u = (double)rand() / RAND_MAX;
    int value = (int)(-log(1-u) / lambda); //% max_delay;
    return value;  
}

/* Function to initialize a matrix node (synapse) */
void initialize_sparse_matrix_node_only(NSGA2Type *nsga2Params, individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    int i;

    // initialize synapse
    matrix_node->value = value; // number of synaptic connections 
    matrix_node->row = row;
    matrix_node->col = col;

    matrix_node->latency = (int8_t *)calloc(abs(value), sizeof(int8_t));
    matrix_node->weight = (double *)calloc(abs(value), sizeof(double));
    matrix_node->learning_rule = (int8_t *)calloc(abs(value), sizeof(int8_t));

    // initialize the values in the list
    for(i = 0; i<abs(value); i++){

        matrix_node->latency[i] = exp_distribution(0.75);//3; 
        if(matrix_node->latency[i] <= 0) 
            matrix_node->latency[i] = 1; // latency must be bigger than 0

        // initialize synapse weight if weights are included in the genotype
        if(nsga2Params->weights_included != 0){
            matrix_node->weight[i] = rndreal(nsga2Params->min_weight, nsga2Params->max_weight);

            if(value < 0) 
                matrix_node->weight[i] = -matrix_node->weight[i]; // if value is negative, the synapse is inhibitory
        }
        
        matrix_node->learning_rule[i] = 0; 
    }

    matrix_node->next_element = NULL;

    // TODO: This should be in another place, but I don't know where
    // add amount of synapses to the individual number of synapses
    ind->n_synapses += abs(value);
}


/**
 * Function to manage input sparse matrix nodes (synapses) in the individual
 */

/* Function to add new synapses and connect them to first neurons (input neurons) */
void initialize_input_synapses(NSGA2Type *nsga2Params, individual *ind){
    int i;

    // allocate memory for the first input synapse
    ind->input_synapses = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));

    // initialize the first synapse
    sparse_matrix_node_t *input_synapse = ind->input_synapses;
    initialize_sparse_matrix_node_only(nsga2Params, ind, ind->input_synapses, 1, 0, 0); // it is only a column, so row -1, and the value is 1 always

    // initialize input synapses
    for(i = 1; i<ind->n_input_synapses; i++){
        input_synapse = initialize_sparse_matrix_node(nsga2Params, ind, input_synapse, 1, 0, i); // it is only a column, so row -1, and the value is 1 always TODO: value shoul be 1 always???
    }
}

/* Function to initialize synapse weights in the SNN structure (I THINK THIS FUNCTION SHOULD BE LOCATED IN ANOTHER FILE) */
void initialize_synapse_weights(NSGA2Type *nsga2Params, individual *ind){
    int i, j; 
    spiking_nn_t *snn = ind->snn;
    lif_neuron_t *neuron;
    synapse_t *synapse;

    // input synapses are located at the begining of the list
    for(i=0; i<snn->n_input_synapses; i++){
        
        synapse = &(snn->synapses[i]);
        
        /*synapse->w = (double)rand() / RAND_MAX;

        // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution // TODO: prob dist.????
        if(synapse->w < nsga2Params->min_weight)
            synapse->w = nsga2Params->min_weight;
        if(synapse->w > nsga2Params->max_weight)
            synapse->w = nsga2Params->max_weight;*/
        
        synapse->w = rndreal(nsga2Params->min_weight, nsga2Params->max_weight);
    }

    // Initialize the rest of synapses
    for(i=0; i<snn->n_neurons; i++){
        // loop over neuron output synapses (weight sign depends on the behaviour of the neuron)
        neuron = &(snn->lif_neurons[i]);
        for(j = 0; j<neuron->n_output_synapse; j++){
            synapse = &(snn->synapses[neuron->output_synapse_indexes[j]]);
            
            //synapse->w = (double)rand() / RAND_MAX;
            synapse->w = rndreal(nsga2Params->min_weight, nsga2Params->max_weight);

            // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution
            /*if(synapse->w < nsga2Params->min_weight)
                synapse->w = nsga2Params->min_weight;
            if(synapse->w > nsga2Params->max_weight)
                synapse->w = nsga2Params->max_weight;*/

            if(neuron->excitatory == -1){
                synapse->w = -synapse->w;
            }
        }
    }
}


/**
 * Function related to dynamic lists that contains the information about the connections between the motifs
 */
int_node_t* initialize_and_allocate_int_node(int value, int_node_t *prev, int_node_t *next){
    int_node_t *int_node = (int_node_t *)calloc(1, sizeof(int_node_t));
    initialize_int_node(int_node, value, prev, next);
    return int_node;
}

/* Function to initialize an int node */
void initialize_int_node(int_node_t *int_node, int value, int_node_t *prev, int_node_t *next){    
    // initialize int node
    int_node->value = value;
    int_node->next = next;
    int_node->prev = prev;

    // update previous and next int nodes linked list connections
    if(prev)
        prev->next = int_node;
    
    if(next)
        next->prev = int_node;
}


/* Function to divide the network in learning zones */
void initialize_learning_zones_population(NSGA2Type *nsga2Params, population *pop, int n_pop){

    int i;

    for(i = 0; i<n_pop; i++){
        printf(" In individual %d\n", i);
        fflush(stdout);
        initialize_learning_zones_individual(nsga2Params, &(pop->ind[i]));
    }
}


/* Function to divide the network in learning zones */
void initialize_learning_zones_individual(NSGA2Type *nsga2Params, individual *ind){

    int i, n_layers;
    new_motif_t **motifs_array;
    new_motif_t *motif_node;
    learning_zone_t *lz;

    // copy motifs from dynamic list to an array for efficiency
    motifs_array = (new_motif_t **)calloc(ind->n_motifs, sizeof(new_motif_t *));
    motif_node = ind->motifs_new;

    // initialize helper arrays // TODO: Probably in this second phase the array of motifs can be helpful all the time
    for(i = 0; i<ind->n_motifs; i++){
        motifs_array[i] = motif_node;
        motif_node->in_lz = 0; // motif not in any learning zone
        motif_node->lz_index = -1; // motif does not have any learning zone asigned yet

        motif_node = motif_node->next_motif;
    }

    
    // loop over motifs to create learning zones
    lz = NULL;
    for(i = 0; i<ind->n_motifs; i++){

        // if motif is not added to any learning zone, then initialize a learning zone
        if(motifs_array[i]->in_lz == 0){ // if motif is not part of any learning zone, then add to one
            
            // initialize learning zone for the dynamic list of zones
            if(ind->n_learning_zones == 0){
                // if it is the first element of the list, allocate memory and initialize
                ind->learning_zones = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));
                lz = ind->learning_zones;
                initialize_learning_zone(nsga2Params, ind, lz, NULL);
            }
            else{
                // if the first element is already initialized, then allocate a new zone and initialize it
                lz = initialize_and_allocate_learning_zone(nsga2Params, ind, lz, lz);
            }
            
            // select the number of layers  for extending the learning zone
            n_layers = rnd(1, 1); // TODO: this should depend on amount of neurons and connectivity
            
            // propagate the learning zone
            extend_learning_zone(nsga2Params, ind, lz, ind->n_learning_zones - 1, i, motifs_array, n_layers, 0); // 0 is used for both directions, input and output
        }
    }

    // cp 
    cp_learning_rules_in_synapses(nsga2Params, ind, motifs_array);


    // free allocated memory
    free(motifs_array);
}

/// @brief add motif to a learning zone and update old learning zone information
/// @param nsga2Params 
/// @param ind 
/// @param lz 
/// @param motif 
/// @return new index of the learning zone
void add_motif_to_learning_zone(NSGA2Type *nsga2Params, individual *ind, learning_zone_t *lz, int lz_index, new_motif_t *motif){

    int helper_index;

    // add the motif to the learning zone if it is not already part of that learning zone
    if(motif->lz_index != lz_index){

        // add one motig to the learning zone 
        lz->n_motifs ++;

        // if motif has a learning zone, then update data of that zone
        if(motif->lz)
            motif->lz->n_motifs --;

        // update motif learning zone
        motif->lz_index = lz_index;
        motif->lz = lz;    
    }

    // motif added to learning zone in this generation or initialization
    motif->in_lz = 1;
}

/// @brief 
/// @param nsga2Params 
/// @param ind 
/// @param lz 
/// @param motif 
void remove_motif_from_learning_zone(NSGA2Type *nsga2Params, individual *ind, learning_zone_t *lz, new_motif_t *motif){
    
    // remove one motif from the learning zone
    lz->n_motifs--;

    // motif does not has any learning zone now
    motif->lz = NULL;
    motif->in_lz = 0;
    motif->lz_index = -1;
}

/// @brief 
/// @param nsga2Params 
/// @param ind 
int_array_t* clear_learning_zones(NSGA2Type *nsga2Params, individual *ind){

    int i, next;
    learning_zone_t *lz, *tmp_lz;
    int_array_t *r_lz;

    r_lz = (int_array_t *)calloc(1, sizeof(int_array_t));
    r_lz->array = (int *)calloc(ind->n_learning_zones, sizeof(int));
    r_lz->n = 0;

    lz = ind->learning_zones;

    // loop over all learning zones
    i = 0;
    next = 0;
    while(lz){
        
        // if learning zone has 0 motifs, remove
        if(lz->n_motifs == 0){

            if(lz == ind->learning_zones){

                ind->learning_zones = lz->next_zone;
                ind->learning_zones->previous_zone = NULL;
            }
            else{

                // make connections
                lz->previous_zone->next_zone = lz->next_zone;
                lz->next_zone->previous_zone = lz->previous_zone;
            }

            // move to the next learning zone and store lz to be removed
            tmp_lz = lz;
            lz = lz->next_zone;

            // store index of the removed learning zone
            r_lz->array[next] = i;
            next++;

            // deallocate memory
            free(tmp_lz->lr);
            free(tmp_lz);
            ind->n_learning_zones --;
        }
        else{
            lz = lz->next_zone;
        }
        i++;
    }

    return r_lz;
}

/// @brief 
/// @param nsga2Params 
/// @param ind 
/// @param r_lz 
void update_learning_zones_indexes(NSGA2Type *nsga2Params, individual *ind, int_array_t *r_lz){

    int i;
    new_motif_t *motif_node;

    motif_node = ind->motifs_new;
    while(motif_node){
        
        i = 0;
        while(r_lz->array[i] < motif_node->lz_index){
            motif_node->lz_index --;
            i++;
        }

        motif_node = motif_node->next_motif;
    }
}


/// @brief 
/// @param nsga2Params 
/// @param lz 
/// @param prev_lz 
/// @param central_motif 
/// @param n_motifs 
/// @return 
learning_zone_t* initialize_and_allocate_learning_zone(NSGA2Type *nsga2Params, individual *ind, learning_zone_t *lz, learning_zone_t *prev_lz){

    // allocate memory for the next learning zone
    lz->next_zone = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));
    lz = lz->next_zone; // move to the next learning zone

    // initialize the learning zone
    initialize_learning_zone(nsga2Params, ind, lz, prev_lz);

    // return learning zone
    return lz;
}

/// @brief 
/// @param nsga2Params 
/// @param lz 
/// @param prev_lz 
/// @param central_motif 
/// @param n_motifs 
void initialize_learning_zone(NSGA2Type *nsga2Params, individual *ind, learning_zone_t *lz, learning_zone_t *prev_lz){
    
    // initialize the learning zone properties
    lz->n_motifs = 0; // it has only one motif yet

    // initialize learning zone
    lz->n_lr = rnd(1, 1); // TODO: this should be changed in the future
    lz->lr = (int *)calloc(lz->n_lr, sizeof(int));
    for(int i=0; i<lz->n_lr; i++){
        lz->lr[i] = rnd(nsga2Params->min_learning_rule, nsga2Params->max_learning_rule);
    }
    
    // dynamic list management
    lz->next_zone = NULL;
    lz->previous_zone = prev_lz;

    // individual has one more learning zone
    ind->n_learning_zones++; // add a learning zone to the individiuals learning zones counter
}

void extend_learning_zone(NSGA2Type *nsga2Params, individual *ind, learning_zone_t *lz, int lz_index, int motif_index, new_motif_t **motifs_array, int remaining_layers, int dir){

    int i, tmp_motif_index;
    new_motif_t *base_motif;
    int_node_t *tmp_motif_int_node;

    // add actual motif to learning zone
    base_motif = motifs_array[motif_index];

    if(base_motif->in_lz == 0){ // add the motif to the learning zone if it is not already in another one // TODO: I'm not very sure with this, revise
        
        add_motif_to_learning_zone(nsga2Params, ind, lz, lz_index, base_motif);

        // expand to n layers in the input and the output
        if(remaining_layers > 0){
        
            if(dir != 1){
                tmp_motif_int_node = ind->connectivity_info.in_connections[motif_index].first_node; // get the first input motif
                for(i=0; i<ind->connectivity_info.in_connections[motif_index].n_nodes; i++){
                    
                    tmp_motif_index = tmp_motif_int_node->value; // get the index of the input/output motif
                    extend_learning_zone(nsga2Params, ind, lz, lz_index, tmp_motif_index, motifs_array, remaining_layers - 1, -1); // -1 is input
                    
                    tmp_motif_int_node = tmp_motif_int_node->next; // go to the next input motif
                }
            }
            if(dir != -1){
                tmp_motif_int_node = ind->connectivity_info.out_connections[motif_index].first_node; // get the first input motif
                for(i=0; i<ind->connectivity_info.out_connections[motif_index].n_nodes; i++){
                    
                    tmp_motif_index = tmp_motif_int_node->value; // get the index of the input/output motif
                    extend_learning_zone(nsga2Params, ind, lz, lz_index, tmp_motif_index, motifs_array, remaining_layers - 1, 1); // 1 is output
                    
                    tmp_motif_int_node = tmp_motif_int_node->next; // go to the next input motif
                }
            }
        }
    }
}

void cp_learning_rules_in_synapses(NSGA2Type *nsga2Params, individual *ind, new_motif_t **motifs_array){
    // copy information of learning rules to synapses
    
    int i, j;
    sparse_matrix_node_t *synapse;
    
    synapse = ind->connectivity_matrix;
    for(i = 0; i<ind->n_motifs; i++){
        for(j = 0; j<ind->n_motifs; j++){
            if(synapse){
                while(synapse && synapse->row < motifs_array[i]->initial_global_index)
                    synapse = synapse->next_element;

                while(synapse && synapse->col < motifs_array[j]->initial_global_index)
                    synapse = synapse->next_element;

                while(synapse && synapse->row >= motifs_array[i]->initial_global_index && synapse->row < motifs_array[i]->initial_global_index + motifs_data[motifs_array[i]->motif_type].n_neurons &&
                    synapse->col >= motifs_array[j]->initial_global_index && synapse->col < motifs_array[j]->initial_global_index + motifs_data[motifs_array[j]->motif_type].n_neurons){
                    
                    synapse->learning_rule[0] = (uint8_t)motifs_array[i]->lz->lr[0];
                    synapse = synapse->next_element;
                }
            }
        }
    }
}