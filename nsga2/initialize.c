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
        #ifdef DEBUG2
        printf(" > Initializing individual %d\n", i);
        #endif

        initialize_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to initialize an individual randomly (memory is allocated here too)*/
void initialize_ind (NSGA2Type *nsga2Params, individual *ind)
{
    // initialize general parameters
    general_initialization(nsga2Params, ind);

    // initialize motifs
    initialize_ind_motifs(ind);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(nsga2Params, ind);

    // set the behaviour for neurons (excitatory or inhibitory)
    set_neurons_behaviour(ind);

    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);

    // connect motifs and construct sparse matrix
    connect_motifs(nsga2Params, ind);

    // initialize input synaptic connections (not included in the sparse matrix)
    initialize_input_synapses(nsga2Params, ind);

    return;
}

/// @brief This function initializes 
///
///
///
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

/*
    Functions related to motifs initilization
*/

/* Function to initialize all motifs from a individual */
void initialize_ind_motifs(individual *ind){
    int i;
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


// I think that I will refactorize this to have three function, initialize and allocate, allocate and initialize
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

/* Function to initilaize the motifs of a network given the motif types */
void initialize_ind_motifs_from_types(individual *ind, int *motif_types){
    int i;
    new_motif_t *motif_node;

    // initialize dynamic list of motifs

    // allocate memory for the first motif
    ind->motifs_new = (new_motif_t *)malloc(sizeof(new_motif_t));
    motif_node = ind->motifs_new;

    // initialize first motif node
    initialize_motif_node_from_type(motif_node, 0, ind, motif_types[0]);

    // initialize the rest of motifs
    for(i = 1; i<ind->n_motifs; i++)
        motif_node = initialize_and_allocate_motif_from_type(motif_node, i, ind, motif_types[i]);
}

/* Function to allocate memory for new motifs and initialize them */
new_motif_t* initialize_and_allocate_motif_from_type(new_motif_t *motif_node, int motif_id, individual *ind, int type){
        // allocate memory
        motif_node->next_motif = (new_motif_t *)malloc(sizeof(new_motif_t));
        motif_node = motif_node->next_motif; // move to the allocated motif 
        
        // initialize
        initialize_motif_node_from_type(motif_node, motif_id, ind, type);

        return motif_node;
}

/* Function to initialize a motif node */
void initialize_motif_node_from_type(new_motif_t *motif, int motif_id, individual *ind, int type){
    // initialize motif data
    motif->motif_id = motif_id;
    motif->motif_type = type;
    motif->initial_global_index = ind->n_neurons;
    motif->next_motif = NULL; // next element is not initialized yet

    // add the amount of neurons of the motif to the individual
    ind->n_neurons += motifs_data[motif->motif_type].n_neurons;
}


/*
    Functions related to neurons initialization
*/

/* Function to initialize all neuron nodes of the neurons list */
void initialize_neuron_nodes(NSGA2Type *nsga2Params, individual *ind){
    int i;
    neuron_node_t *neuron_node;

    // allocate memory for the first element of the neurons list
    ind->neurons = (neuron_node_t *)malloc(sizeof(neuron_node_t));

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
    neuron_node->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t));
    neuron_node = neuron_node->next_neuron;

    // initialize neuron
    initialize_neuron_node(nsga2Params, neuron_node);

    return neuron_node;
}

/* Function to initialize neuron nodes */
void initialize_neuron_node(NSGA2Type *nsga2Params, neuron_node_t *neuron_node){
    neuron_node->threshold = rndreal(nsga2Params->min_vthres, nsga2Params->max_vthres);
    neuron_node->refract_time = rnd(nsga2Params->min_refracttime, nsga2Params->max_refracttime);
    neuron_node->v_rest = rndreal(nsga2Params->min_vrest, nsga2Params->max_vrest);
    neuron_node->r = rnd(nsga2Params->min_R, nsga2Params->max_R);
    neuron_node->next_neuron = NULL; // the next element is not initialized yet
}


/* Function to set the behaviour (excitatory or inhibitory) of the neurons */
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


// TODO: TEST THIS FUNCTION
/* 
    Functions to connect motifs and neurons 
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



/*
    Functions to create synapses between motifs (neurons) and construct the sparse matrix
*/


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

/* Function to randomly selected to which motifs is each motif connected, and then build the sparse matrix */
void connect_motifs(NSGA2Type *nsga2Params, individual *ind){
    int i, j, tmp_motif, valid, tmp_n;

    int n_connections, max_connect_per_motif;

    int *n_input_connections_per_motif; // number of input connections between motifs per each motif
    int *n_output_connections_per_motif; // number of output connections of the actual motif with other motifs

    int *n_input_connections_per_motif_done; // number of input connections between motifs per each motif already done
    int *n_output_connections_per_motif_done; // number of output connections of the actual motif with other motifs already done

    int **selected_input_motifs_per_motif; // list of motifs to connect with each motif. 
    int **selected_output_motifs_per_motif; // list of motifs to connect with each motif. 

    int_array_t *selected_input_motifs, *selected_output_motifs; 
    new_motif_t *motif;

    /* allocate memory for lists */
    n_input_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int));
    n_output_connections_per_motif = (int *)malloc(ind->n_motifs * sizeof(int));

    n_input_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int));
    n_output_connections_per_motif_done = (int *)malloc(ind->n_motifs * sizeof(int));

    selected_input_motifs_per_motif = (int **)malloc(ind->n_motifs * sizeof(int *));
    selected_output_motifs_per_motif = (int **)malloc(ind->n_motifs * sizeof(int *));

    selected_input_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));
    selected_output_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));



    /* set the maximum number of input and output connections per each motif */
    int min_val, max_val;
    min_val = (int)(ind->n_motifs * 0.25);
    max_val = (int)(ind->n_motifs * 0.50);
    max_connect_per_motif = rnd(min_val, max_val); // this is the maximum number of connections that a motif can contain as input or output //rnd(2, 5);
    max_connect_per_motif = rnd(2,5);
    /* Initialize the lists */
    for(i = 0; i<ind->n_motifs; i++){
        // set the amount of input and output connections for the motif 
        n_input_connections_per_motif[i] = max_connect_per_motif; // I am not very convinced with this
        n_output_connections_per_motif[i] = max_connect_per_motif;

        // not connections done yet
        n_input_connections_per_motif_done[i] = 0;
        n_output_connections_per_motif_done[i] = 0;
    }


    //===================================================================================================
    // TODO: Some things here should be refactorized to a function: 

    // make some randomization to change the amount of input and output connections for each motifs
    randomize_motif_connections(ind, n_input_connections_per_motif);
    randomize_motif_connections(ind, n_output_connections_per_motif);


    /* allocate memory for selected input and output motifs for each motif */
    for(i = 0; i<ind->n_motifs; i++){
        selected_input_motifs_per_motif[i] = (int *)malloc(n_input_connections_per_motif[i] * sizeof(int));
        selected_output_motifs_per_motif[i] = (int *)malloc(n_output_connections_per_motif[i] * sizeof(int));
    }


    /* connect the motifs with their input and output motifs */
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

            // add the motif as input and output for the corresponding motifs
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


        // copy the information to the int_array_t struct
        selected_input_motifs[i].n = n_input_connections_per_motif[i];
        selected_input_motifs[i].array = (int *)malloc(selected_input_motifs[i].n * sizeof(int));

        for(j = 0; j<selected_input_motifs[i].n; j++)
            selected_input_motifs[i].array[j] = selected_input_motifs_per_motif[i][j];
        
        selected_output_motifs[i].n = n_output_connections_per_motif[i];
        selected_output_motifs[i].array = (int *)malloc(selected_output_motifs[i].n * sizeof(int));
        
        for(j = 0; j<selected_output_motifs[i].n; j++)
            selected_output_motifs[i].array[j] = selected_output_motifs_per_motif[i][j];
    }


    /* Print information about connections */
    // print information
    /*int sum_input_connections=0, sum_output_connections=0;
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
    }*/




    // copy information of connections to int_array_t structs
    //motif = ind->motifs_new;

    /*for(i = 0; i<ind->n_motifs; i++){
        selected_input_motifs[i].n = n_input_connections_per_motif[i];
        selected_input_motifs[i].array = (int *)malloc(selected_input_motifs[i].n * sizeof(int));

        for(j = 0; j<selected_input_motifs[i].n; j++)
            selected_input_motifs[i].array[j] = selected_input_motifs_per_motif[i][j];
        
        selected_output_motifs[i].n = n_output_connections_per_motif[i];
        selected_output_motifs[i].array = (int *)malloc(selected_output_motifs[i].n * sizeof(int));
        
        for(j = 0; j<selected_output_motifs[i].n; j++)
            selected_output_motifs[i].array[j] = selected_output_motifs_per_motif[i][j];
    }*/

    

    // TODO: THIS SHOULD BE REFACTORIZED, AS TOO MUCH COPIES ARE DONE ACTUALLY

    // Copy information to dynamic lists of individuals (ALL THIS MUST BE REFACTORIZED AND OPTIMIZED AS MOST COPIES NOW ARE USELESS - THIS ONE IS WHAT WILL REMAIN)
    int_node_t *int_node;
    
    // allocate memory for dynamic lists to store motif connectivity information. Larger lists than neccessary are allocated as network will raise
    ind->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind->connectivity_info.n_max_motifs = ind->n_motifs * 10; 

    // loop and initialize all int nodes (THIS CAN BE MOVED TO A FUNCTION???)
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


    /* Construct the connectivity sparse matrix by a dynamic list */
    #ifdef DEBUG2
    printf(" > > > Building sparse matrix\n");
    #endif
    
    new_construct_sparse_matrix(ind, selected_input_motifs);
    
    #ifdef DEBUG2
    printf(" > > > Sparse matrix built!\n");
    #endif


    /* free memory allocated by lists */
    deallocate_int_array(selected_input_motifs, ind->n_motifs);
    deallocate_int_array(selected_output_motifs, ind->n_motifs);
    
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


// This function initializes an int node setting the value and the references to the previous and the next int nodes
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


int_node_t* initialize_and_allocate_int_node(int value, int_node_t *prev, int_node_t *next){
    int_node_t *int_node = (int_node_t *)malloc(sizeof(int_node_t));
    initialize_int_node(int_node, value, prev, next);
    return int_node;
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

/*sparse_matrix_node_t* construct_sparse_matrix_motif_columns(individual *ind, new_motif_t *motif1, new_motif_t *motif2, sparse_matrix_node_t matrix_node, int n_selected_input_motifs, int *selected_input_motifs){

    int i, j, s, k, s;
    int motif1_type, motif2_type, motif1_first_neuron_global_index, motif2_first_neuron_global_index;

    // Loop over the neurons of the motif (j is used as the local index of the neuron inside the motif)
    for(j = 0; j<motif_info->n_neurons; j++){ 
        neuron_index = first_neuron_global_index + j; // get the global index of the neuron being computed
        actual_motif_processed = 0; // the actual motif internal structure data has not been added to the sparse matrix

        //Loop over the motifs connected to the actual one to construct it column by column 
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
*/


/* New functions to construct the sparse matrix */

void new_construct_sparse_matrix(individual *ind, int_array_t *selected_input_motifs){

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
    first_synapse_node = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t)); // this is only used to store the pointer of the first synapse node of the matrix, then, the first is ignored as the real matrix starts in the next elemnet
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
        synapse_node = construct_motif_sparse_matrix_columns(ind, synapse_node, &(selected_input_motifs[i]), &SMBI);

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

sparse_matrix_node_t* construct_motif_sparse_matrix_columns(individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI){
    
    // loop over motif neurons
    for(int i=0; i<SMBI->actual_motif_info->n_neurons; i++){
        SMBI->actual_neuron_local_index = i;
        synapse_node = construct_neuron_sparse_matrix_column(ind, synapse_node, selected_input_motifs, SMBI);
    }
    return synapse_node;
}

sparse_matrix_node_t* construct_neuron_sparse_matrix_column(individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI){
    
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

        synapse_node = add_neuron_to_motif_connections(ind, synapse_node, SMBI);
    }
    return synapse_node;
}

sparse_matrix_node_t* add_neuron_to_motif_connections(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
    int i, j;
    int actual_motif_index, input_motif_index, previous_input_motif_index, next_input_motif_index;

    actual_motif_index = SMBI->actual_motif_index;
    input_motif_index = SMBI->input_motif_index;
    previous_input_motif_index = SMBI->previous_input_motif_index;
    next_input_motif_index = SMBI->next_input_motif_index;

    // Check different cases to construct the structure

    if (input_motif_index == actual_motif_index) {
        // loop over motif neurons
        synapse_node = build_motif_internal_structure_and_connection(ind, synapse_node, SMBI);
    }
    else if (input_motif_index < actual_motif_index) {
        // loop over motif neurons
        synapse_node = build_connection(ind, synapse_node, SMBI);
        
        if(next_input_motif_index == -1){
            synapse_node = build_motif_internal_structure(ind, synapse_node, SMBI);
        }
    }
    else {// (input_motif_index > actual_motif_index) {
        // loop over motif neurons
        if(previous_input_motif_index < actual_motif_index && SMBI->actual_motif_proccessed == 0){ 
            synapse_node = build_motif_internal_structure(ind, synapse_node, SMBI);
        }

        synapse_node = build_connection(ind, synapse_node, SMBI);
    }
    return synapse_node;
}

sparse_matrix_node_t* build_motif_internal_structure(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
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

            synapse_node = initialize_sparse_matrix_node(ind, synapse_node, value, row, col);
        }
    }
    return synapse_node;
}

sparse_matrix_node_t* build_connection(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
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
                if(row < 0){
                    printf(" NEGATIVE!!!\n");
                    fflush(stdout);
                }
                col = actual_motif_first_neuron_index + actual_neuron_local_index; // we are processing the neuron_local_index.th neuron of the actual motif

                synapse_node = initialize_sparse_matrix_node(ind, synapse_node, value, row, col);
            }
        }
    }
    return synapse_node;
}

sparse_matrix_node_t* build_motif_internal_structure_and_connection(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI){
    
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

            synapse_node = initialize_sparse_matrix_node(ind, synapse_node, value, row, col);
        }
    }
    return synapse_node;
}

/* Function to allocate memory for the new sparse matrix node and initialize it */
sparse_matrix_node_t* initialize_sparse_matrix_node(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col){
    // allocate memory for the synapse
    matrix_node->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
    matrix_node = matrix_node->next_element; // move to the next synapse

    //printf(" > > > > > > > > Initializing node value %d, row %d, col %d\n", value, row, col);

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


// TODO: This function must be revised, changed, and tested
/* Function to add new synapses to an already constructed sparse matrix
    - ind: individual
    - n_new_motifs: number of new motifs added to the network
    - motif_node: the first new motif in the motif list
    - input_motif: selected input motifs for each new motif
    - output_motifs: selected output motifs for each new motif
*/
void construct_semi_sparse_matrix(individual *ind, int n_new_motifs, int_array_t *new_motifs_output_motifs){
    int i, j, k, s, l, m;
    int row, col, value;

    // The following two lists are auxiliary lists used to store motifs types and motifs first neurons global indexes as the dynamic list can not be
    // indexed
    int *motifs_types_list, *new_motifs_types_list; // list to store the types of the new motifs that will be added
    int *motifs_neurons_global_index_list, *new_motifs_neurons_initial_global_indexes; // list to store the global indexes of the first neuron for each new motif

    // Other aux variables
    new_motif_t *tmp_motif_node;
    motif_t *motif_info, *tmp_motif_info;
    sparse_matrix_node_t *matrix_node, *tmp_matrix_node;
    int_array_t *pre_motifs_new_input;
    int motif_type, first_neuron_global_index, neuron_index, tmp_motif_type, tmp_first_neuron_global_index, tmp_motif_index, tmp_neuron_index;
    int counter = 0;
    int actual_motif_processed = 0;

    int actual_column;

    /* Variables initialization */
    tmp_motif_node = ind->motifs_new;

    // initialize first sparse matrix node
    matrix_node = ind->connectivity_matrix; // get the first element of the matrix


    // allocate memory for aux lists and initialize
    motifs_types_list = (int *)malloc((ind->n_motifs - n_new_motifs) * sizeof(int));
    motifs_neurons_global_index_list = (int *)malloc((ind->n_motifs - n_new_motifs) * sizeof(int));
    new_motifs_types_list = (int *)malloc(n_new_motifs * sizeof(int));
    new_motifs_neurons_initial_global_indexes = (int *)malloc(n_new_motifs * sizeof(int));

    // Store motifs types and first neuron global indexes
    for(i = 0; i<ind->n_motifs - n_new_motifs; i++){
        motifs_types_list[i] = tmp_motif_node->motif_type; // store the motif type
        motifs_neurons_global_index_list[i] = tmp_motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        tmp_motif_node = tmp_motif_node->next_motif;
    }
    // store information of new motifs
    for(i = ind->n_motifs - n_new_motifs; i<ind->n_motifs; i++){
        new_motifs_types_list[i - ind->n_motifs] = tmp_motif_node->motif_type; // store the motif type
        new_motifs_neurons_initial_global_indexes[i - ind->n_motifs] = tmp_motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        tmp_motif_node = tmp_motif_node->next_motif;
    }

    /* Allocate memory for */
    // Store the new input motifs for each previously existing motif (using output_motifs list of new motifs)
    pre_motifs_new_input = (int_array_t *)calloc(ind->n_motifs - n_new_motifs, sizeof(int_array_t));
    for(i = 0; i<ind->n_motifs - n_new_motifs; i++){
        pre_motifs_new_input[i].array = (int *)calloc(n_new_motifs, sizeof(int)); // memory for the maximum possible number of motifs is allocated
    }

    // Loop over new_motifs to find the new input motifs of old motifs
    int helper_motif_index;
    for(i = 0; i<n_new_motifs; i++){
        for(j=0; j<new_motifs_output_motifs[i].n; j++){

            helper_motif_index = new_motifs_output_motifs[i].array[j]; // get the motif that the new one is connected to
            pre_motifs_new_input[helper_motif_index].array[pre_motifs_new_input[helper_motif_index].n] = i; // add the new connection to the previously existing motif list 
            pre_motifs_new_input[helper_motif_index].n ++;

        }
    }


    /* 1. First loop over existing motifs to add new input connections */
    /* 2. Second, loop over new motifs to add new columns in the sparse matrix */

    /* Loop over previously existing motifs to update columns and to add new rows related to new motifs */
    for(i=0; i<ind->n_motifs - n_new_motifs; i++){
        // Get data about the motif that is being processed
        motif_type = motifs_types_list[i]; // get the motif type
        motif_info = &(motifs_data[motif_type]); // get the structure with the motif information
        first_neuron_global_index = motifs_neurons_global_index_list[i]; // get the global index of the first neuron of the motif
        

        // check if this motif has new input motifs, if not, move to the next motif as there is nothing to process
        if(pre_motifs_new_input[i].n > 0){
            
            // loop over actual motif neurons to add new connections
            for(s = 0; s<motif_info->n_neurons; s++){
                
                // move to the last row of the column of this neuron
                while(matrix_node->col == matrix_node->next_element->col){
                    matrix_node = matrix_node->next_element;
                }
                
                // store the first element of the next column
                tmp_matrix_node = matrix_node->next_element;
                
                // check if the neuron is an input neuron to add the new connections, else it is not neccessary
                if(check_if_neuron_is_input(motif_type, s) == 1){
                    
                    // loop over new input motifs of this motif to add new connections
                    for(k = 0; k<pre_motifs_new_input[i].n; k++){
                        // get input motif information
                        tmp_motif_type = new_motifs_types_list[pre_motifs_new_input[i].array[k]]; // get the motif type
                        tmp_motif_info = &(motifs_data[tmp_motif_type]); // get the structure with the motif information
                        tmp_first_neuron_global_index = new_motifs_neurons_initial_global_indexes[pre_motifs_new_input[i].array[k]]; // get the global index of the first neuron of the motif
                        
                        // count how much times this motif is connected to the one being computed
                        counter = 1;
                        while(l < pre_motifs_new_input[i].n - 1 && 
                            pre_motifs_new_input[i].array[l] == pre_motifs_new_input[i].array[l+1]){ // if we are in the last it can no be repited
                                counter ++; // there is a repetition
                                l++; // move forward in k
                        }                        

                        // loop over input motif neurons
                        for(l=0; l<tmp_motif_info->n_neurons; l++){
                            // check if the neuron is an output neuron to make the connection
                            if(check_if_neuron_is_output(tmp_motif_type, l)==1){
                                // add new element to the sparse matrix
                                value = counter;
                                row = ind->n_motifs - n_new_motifs + pre_motifs_new_input[i].array[k] + l;
                                col = first_neuron_global_index + s;
                                matrix_node = initialize_sparse_matrix_node(ind, matrix_node, value, row, col);
                            }
                        }
                    }
                }

                // make the connection again with the next column
                matrix_node->next_element = tmp_matrix_node;
            }
        }
        // loop until the next motif is reached
        else{
            // move until we reach to the first column of the next motif
            while(matrix_node->col < first_neuron_global_index + motif_info->n_neurons){
                matrix_node = matrix_node->next_element;
            }
        }
    }


    /* Loop over new motifs to add entire columns of those motifs */
    for(i=ind->n_motifs - n_new_motifs; i<ind->n_motifs; i++){
        // Get data about the motif that is being processed
        motif_type = motifs_types_list[i]; // get the motif type
        motif_info = &(motifs_data[motif_type]); // get the structure with the motif information
        first_neuron_global_index = motifs_neurons_global_index_list[i]; // get the global index of the first neuron of the motif
    }
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


/* Function to add new synapses and connect them to first neurons (input neurons) */
void initialize_input_synapses(NSGA2Type *nsga2Params, individual *ind){
    int i;

    sparse_matrix_node_t *input_synapse;

    // allocate memory for the first input synapse
    ind->input_synapses = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    // initialize the first synapse
    input_synapse = ind->input_synapses;
    initialize_sparse_matrix_node_only(ind, ind->input_synapses, 1, 0, 0); // it is only a column, so row -1, and the value is 1 always

    // initialize input synapses
    for(i = 1; i<ind->n_input_synapses; i++){
        input_synapse = initialize_sparse_matrix_node(ind, input_synapse, 1, 0, i); // it is only a column, so row -1, and the value is 1 always TODO: value shoul be 1 always???
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
        synapse->w = (double)rand() / RAND_MAX;

        // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution
        if(synapse->w < nsga2Params->min_weight)
            synapse->w = nsga2Params->min_weight;
        if(synapse->w > nsga2Params->max_weight)
            synapse->w = nsga2Params->max_weight;
    }

    // Initialize the rest of synapses
    for(i=0; i<snn->n_neurons; i++){
        // loop over neuron output synapses (weight sign depends on the behaviour of the neuron)
        neuron = &(snn->lif_neurons[i]);
        for(j = 0; j<neuron->n_output_synapse; j++){
            synapse = &(snn->synapses[neuron->output_synapse_indexes[j]]);
            
            synapse->w = (double)rand() / RAND_MAX;

            // TODO: this is temporal, should be changed in the near future, using some intelligent weight initialization or a distribution
            if(synapse->w < nsga2Params->min_weight)
                synapse->w = nsga2Params->min_weight;
            if(synapse->w > nsga2Params->max_weight)
                synapse->w = nsga2Params->max_weight;

            if(neuron->excitatory == -1){
                synapse->w = -synapse->w;
            }
        }
    }
}
