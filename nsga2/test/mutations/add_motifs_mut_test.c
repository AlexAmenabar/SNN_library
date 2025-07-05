# include "nsga2.h"
# include "snn_library.h"

# define KRED "\x1B[31m"
# define KGRN "\x1B[32m"


void initialize_ind_test (individual *ind, int *motif_types, int_array_t *selected_input_motifs, int n_motifs)
{
    // initialize general parameters
    //general_initialization(nsga2Params, ind);
    ind->n_motifs = n_motifs;

    ind->n_neurons = 0;
    //ind->n_input_neurons = 2; 

    ind->n_synapses = 0;
    //ind->n_input_synapses = 2;


    // initialize motifs
    initialize_ind_motifs_from_types(ind, motif_types);
    
    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    set_neurons_behaviour(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);

    // connect motifs and construct sparse matrix
    //connect_motifs(ind);
    new_construct_sparse_matrix(ind, selected_input_motifs);

    // initialize input synaptic connections (not included in the sparse matrix)
    //initialize_input_synapses(nsga2Params, ind);

    return;
}



/* This mutation adds a new motif (or more than one) to the network (with random connections) */
// This version of the function removes random execution by deterministic ones to test
void add_motif_mutation_test(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs, int type){
    int i = 0, j;

    int last_motif_id; // vairables to store the number of new motifs that are added to the genotype and the id of the last processed motif
    new_motif_t *motif_node; // motif nodes to store the new motifs and to loop over the dynamic list of motifs
    neuron_node_t *neuron_node; // neuron node to loop over the dynamic list of neurons and to add new ones
    int min_connected_motifs, max_connected_motifs;
    int_array_t *all_selected_input_motifs; // lists to store the selected input and output motifs for the new motif

    // add the number of new motifs to the individual
    ind->n_motifs += n_new_motifs;

    // get the first motif and synapse
    motif_node = ind->motifs_new; // first motif
    neuron_node = ind->neurons;


    // loop until the last motif is acquited
    while(motif_node->next_motif != NULL)
        motif_node = motif_node->next_motif;

    // store last motif id, as the new motif id will be the following one
    last_motif_id = motif_node->motif_id;
    

    // loop over the neurons until the last is reached
    while(neuron_node->next_neuron != NULL)
        neuron_node = neuron_node->next_neuron;


    // add new motifs, and for each motif, its neurons, to the dynamic lists
    for(i = 0; i<n_new_motifs; i++){
        // update motif id for the next one
        last_motif_id ++;

        // allocate memory for the next motif (the new one)
        motif_node = initialize_and_allocate_motif_from_type(motif_node, last_motif_id, ind, type);

        // add motif neurons to the dynamic list of neurons
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++)
            neuron_node = initialize_and_allocate_neuron_node(neuron_node);
    }

    /* Select input and output motifs for each new motif, and then map all to*/
    all_selected_input_motifs = map_IO_motifs_to_input(ind, n_new_motifs, selected_input_motifs, selected_output_motifs); // map

    /* Add the new synaptic connectiosn to the connections list */
    update_sparse_matrix_add_motifs(ind, n_new_motifs, all_selected_input_motifs);
}


int compare_target_and_obtained_matrix(int *result_matrix, int *target_matrix, int n_neurons){

    int i, j, equals = 1;
    
    for(i = 0; i<n_neurons; i++){
        for(j = 0; j<n_neurons; j++){
            if(result_matrix[i*n_neurons+j] != target_matrix[i*n_neurons+j])
                equals = 0;
        }
    }

    return equals;
}

// General test, with one doble connection between motifs
int test1(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs, *new_selected_input_motifs, *new_selected_output_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;
    int n_new_motifs;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(3 * sizeof(int));

    // configure test
    ind->n_motifs = 3;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    //motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(3 * sizeof(int_array_t));
    selected_input_motifs[0].n = 2;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 2;

    selected_input_motifs[0].array = (int *)malloc(2 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(2 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;


    // initialize motifs dynamic list
    //initialize_ind_motifs_from_types(ind, motif_types);
    initialize_ind_test (ind, motif_types, selected_input_motifs, ind->n_motifs);

    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

    target_matrix[1] = 1;
    target_matrix[13] = 1;
    target_matrix[15] = 1;

    target_matrix[19] = 1;
    target_matrix[28] = 1;
    target_matrix[32] = 2;
    target_matrix[34] = -1;
    target_matrix[37] = 1;
    target_matrix[39] = 1;
    /*target_matrix[52] = 1;
    target_matrix[53] = 1;
    target_matrix[54] = 1;*/

    target_matrix[42] = 1;
    target_matrix[46] = 1;
    target_matrix[55] = -1;
    target_matrix[58] = 1;

    /*target_matrix[88] = 1;
    target_matrix[93] = 1;
    target_matrix[95] = 1;
    target_matrix[97] = 1;
    target_matrix[98] = 1;
    target_matrix[99] = 1;
    target_matrix[104] = 1;
    target_matrix[106] = 1;
    target_matrix[107] = 1;
    target_matrix[109] = 1;
    target_matrix[110] = 1;
    target_matrix[115] = 1;
    target_matrix[117] = 1;
    target_matrix[118] = 1;
    target_matrix[119] = 1;*/


    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

    n_new_motifs = 1;

    new_selected_input_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));
    new_selected_output_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));

    new_selected_input_motifs[0].n = 1;
    new_selected_output_motifs[0].n = 2;

    new_selected_input_motifs[0].array = (int *)malloc(sizeof(int));
    new_selected_output_motifs[0].array = (int *)malloc(2 * sizeof(int));

    new_selected_input_motifs[0].array[0] = 1;
    new_selected_output_motifs[0].array[0] = 0;
    new_selected_output_motifs[0].array[1] = 2;

    // 14
    //printf("%d\n", ind->n_synapses);

    // now add new motifs and connections and test
    NSGA2Type nsga2Params;
    add_motif_mutation_test(&nsga2Params, ind, n_new_motifs, new_selected_input_motifs, new_selected_output_motifs, 6);

    free(result_matrix);
    free(target_matrix);
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

    target_matrix[1] = 1;
    target_matrix[16] = 1;
    target_matrix[18] = 1;

    target_matrix[25] = 1;
    target_matrix[37] = 1;
    target_matrix[44] = 2;
    target_matrix[46] = -1;
    target_matrix[49] = 1;
    target_matrix[51] = 1;
    target_matrix[52] = 1;
    target_matrix[53] = 1;
    target_matrix[54] = 1;

    target_matrix[57] = 1;
    target_matrix[61] = 1;
    target_matrix[73] = -1;
    target_matrix[79] = 1;

    target_matrix[88] = 1;
    target_matrix[93] = 1;
    target_matrix[95] = 1;
    target_matrix[97] = 1;
    target_matrix[98] = 1;
    target_matrix[99] = 1;
    target_matrix[104] = 1;
    target_matrix[106] = 1;
    target_matrix[107] = 1;
    target_matrix[109] = 1;
    target_matrix[110] = 1;
    target_matrix[115] = 1;
    target_matrix[117] = 1;
    target_matrix[118] = 1;
    target_matrix[119] = 1;


    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);


# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // if not equals. return
    if(equals == 1){
        
        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 2 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 5 && motif_node->next_motif->next_motif->initial_global_index != 5 
            || motif_node->next_motif->next_motif->next_motif->motif_type != 6 && motif_node->next_motif->next_motif->next_motif->initial_global_index != 8 
            || motif_node->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 4 || ind->n_neurons != 11 || ind->n_synapses != 32){
            equals = 0;
        }

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons){
            equals = 0;        
        }
    }
    
    return equals;
}

int test2(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs, *new_selected_input_motifs, *new_selected_output_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;
    int n_new_motifs;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(3 * sizeof(int));

    // set motif types
    motif_types[0] = 0;
    motif_types[1] = 0;
    motif_types[2] = 0;

    selected_input_motifs = (int_array_t *)malloc(2 * sizeof(int_array_t));
    selected_input_motifs[0].n = 6;
    selected_input_motifs[1].n = 6;
    selected_input_motifs[2].n = 6;

    selected_input_motifs[0].array = (int *)malloc(6 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(6 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(6 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;
    selected_input_motifs[0].array[2] = 1;
    selected_input_motifs[0].array[3] = 1;
    selected_input_motifs[0].array[4] = 2;
    selected_input_motifs[0].array[5] = 2;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 0;
    selected_input_motifs[1].array[2] = 1;
    selected_input_motifs[1].array[3] = 1;
    selected_input_motifs[1].array[4] = 2;
    selected_input_motifs[1].array[5] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 0;
    selected_input_motifs[2].array[2] = 1;
    selected_input_motifs[2].array[3] = 1;
    selected_input_motifs[2].array[4] = 2;
    selected_input_motifs[2].array[5] = 2;


    // initialize motifs dynamic list
    //initialize_ind_motifs_from_types(ind, motif_types);
    ind->n_motifs = 3;
    initialize_ind_test (ind, motif_types, selected_input_motifs, ind->n_motifs);

    // ============= //
    // add new motif //
    // ============= //

    n_new_motifs = 1;

    new_selected_input_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));
    new_selected_output_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));

    new_selected_input_motifs[0].n = 2;
    new_selected_output_motifs[0].n = 3;

    new_selected_input_motifs[0].array = (int *)malloc(2 * sizeof(int));
    new_selected_output_motifs[0].array = (int *)malloc(3 * sizeof(int));

    // set motifs
    new_selected_input_motifs[0].array[0] = 0;
    new_selected_input_motifs[0].array[1] = 3;

    new_selected_output_motifs[0].array[0] = 0;
    new_selected_output_motifs[0].array[1] = 0;
    new_selected_output_motifs[0].array[2] = 1;


    // now add new motifs and connections and test
    NSGA2Type nsga2Params;
    add_motif_mutation_test(&nsga2Params, ind, n_new_motifs, new_selected_input_motifs, new_selected_output_motifs, 0);

    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));


    target_matrix[1] = 1;
    target_matrix[8] = 2;
    target_matrix[10] = 2;
    target_matrix[12] = 2;
    target_matrix[14] = 1;

    target_matrix[19] = 1;
    target_matrix[24] = 2;
    target_matrix[26] = 2;
    target_matrix[28] = 2;

    target_matrix[37] = 1;
    target_matrix[40] = 2;
    target_matrix[42] = 2;
    target_matrix[44] = 2;

    target_matrix[55] = 1;
    target_matrix[56] = 2;
    target_matrix[58] = 1;
    target_matrix[62] = 1;

    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);


# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // if not equals. return
    if(equals == 1){
        
        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 0 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->initial_global_index != 4 
            || motif_node->next_motif->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->next_motif->initial_global_index != 6 
            || motif_node->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 4 || ind->n_neurons != 8 || ind->n_synapses != 27){
            equals = 0;
        }

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons){
            equals = 0;        
        }
    }
    
    return equals;
}

int test3(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs, *new_selected_input_motifs, *new_selected_output_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;
    int n_new_motifs;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(2 * sizeof(int));

    // set motif types
    motif_types[0] = 0;
    motif_types[1] = 0;

    selected_input_motifs = (int_array_t *)malloc(2 * sizeof(int_array_t));
    selected_input_motifs[0].n = 4;
    selected_input_motifs[1].n = 4;

    selected_input_motifs[0].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(4 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;
    selected_input_motifs[0].array[2] = 1;
    selected_input_motifs[0].array[3] = 1;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 0;
    selected_input_motifs[1].array[2] = 1;
    selected_input_motifs[1].array[3] = 1;


    // initialize motifs dynamic list
    //initialize_ind_motifs_from_types(ind, motif_types);
    ind->n_motifs = 2;
    initialize_ind_test (ind, motif_types, selected_input_motifs, ind->n_motifs);

    // ============= //
    // add new motif //
    // ============= //

    n_new_motifs = 2;

    new_selected_input_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));
    new_selected_output_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));

    new_selected_input_motifs[0].n = 6;
    new_selected_output_motifs[0].n = 4;

    new_selected_input_motifs[1].n = 2;
    new_selected_output_motifs[1].n = 3;

    new_selected_input_motifs[0].array = (int *)malloc(6 * sizeof(int));
    new_selected_output_motifs[0].array = (int *)malloc(4 * sizeof(int));

    new_selected_input_motifs[1].array = (int *)malloc(2 * sizeof(int));
    new_selected_output_motifs[1].array = (int *)malloc(3 * sizeof(int));

    // set motifs
    new_selected_input_motifs[0].array[0] = 0;
    new_selected_input_motifs[0].array[1] = 0;
    new_selected_input_motifs[0].array[2] = 1;
    new_selected_input_motifs[0].array[3] = 1;
    new_selected_input_motifs[0].array[4] = 2;
    new_selected_input_motifs[0].array[5] = 2;

    new_selected_output_motifs[0].array[0] = 0;
    new_selected_output_motifs[0].array[1] = 0;
    new_selected_output_motifs[0].array[2] = 1;
    new_selected_output_motifs[0].array[3] = 1;


    new_selected_input_motifs[1].array[0] = 0;
    new_selected_input_motifs[1].array[1] = 3;

    new_selected_output_motifs[1].array[0] = 0;
    new_selected_output_motifs[1].array[1] = 0;
    new_selected_output_motifs[1].array[2] = 1;


    // now add new motifs and connections and test
    NSGA2Type nsga2Params;
    add_motif_mutation_test(&nsga2Params, ind, n_new_motifs, new_selected_input_motifs, new_selected_output_motifs, 0);

    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));


    target_matrix[1] = 1;
    target_matrix[8] = 2;
    target_matrix[10] = 2;
    target_matrix[12] = 2;
    target_matrix[14] = 1;

    target_matrix[19] = 1;
    target_matrix[24] = 2;
    target_matrix[26] = 2;
    target_matrix[28] = 2;

    target_matrix[37] = 1;
    target_matrix[40] = 2;
    target_matrix[42] = 2;
    target_matrix[44] = 2;

    target_matrix[55] = 1;
    target_matrix[56] = 2;
    target_matrix[58] = 1;
    target_matrix[62] = 1;

    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);


# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // if not equals. return
    if(equals == 1){
        
        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 0 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->initial_global_index != 4 
            || motif_node->next_motif->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->next_motif->initial_global_index != 6 
            || motif_node->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 4 || ind->n_neurons != 8 || ind->n_synapses != 27){
            equals = 0;
        }

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons){
            equals = 0;        
        }
    }
    
    return equals;
}

int test4(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs, *new_selected_input_motifs, *new_selected_output_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;
    int n_new_motifs;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(2 * sizeof(int));

    // set motif types
    motif_types[0] = 0;
    motif_types[1] = 0;

    selected_input_motifs = (int_array_t *)malloc(2 * sizeof(int_array_t));
    selected_input_motifs[0].n = 4;
    selected_input_motifs[1].n = 4;

    selected_input_motifs[0].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(4 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;
    selected_input_motifs[0].array[2] = 1;
    selected_input_motifs[0].array[3] = 1;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 0;
    selected_input_motifs[1].array[2] = 1;
    selected_input_motifs[1].array[3] = 1;


    // initialize motifs dynamic list
    //initialize_ind_motifs_from_types(ind, motif_types);
    ind->n_motifs = 2;
    initialize_ind_test (ind, motif_types, selected_input_motifs, ind->n_motifs);

    // ============= //
    // add new motif //
    // ============= //

    n_new_motifs = 2;

    new_selected_input_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));
    new_selected_output_motifs = (int_array_t *)malloc(n_new_motifs * sizeof(int_array_t));

    new_selected_input_motifs[0].n = 7;
    new_selected_output_motifs[0].n = 7;

    new_selected_input_motifs[1].n = 3;
    new_selected_output_motifs[1].n = 1;

    new_selected_input_motifs[0].array = (int *)malloc(7 * sizeof(int));
    new_selected_output_motifs[0].array = (int *)malloc(7 * sizeof(int));

    new_selected_input_motifs[1].array = (int *)malloc(3 * sizeof(int));
    new_selected_output_motifs[1].array = (int *)malloc(1 * sizeof(int));

    // set motifs
    new_selected_input_motifs[0].array[0] = 0;
    new_selected_input_motifs[0].array[1] = 0;
    new_selected_input_motifs[0].array[2] = 1;
    new_selected_input_motifs[0].array[3] = 1;
    new_selected_input_motifs[0].array[4] = 2;
    new_selected_input_motifs[0].array[5] = 2;
    new_selected_input_motifs[0].array[6] = 3;

    new_selected_output_motifs[0].array[0] = 0;
    new_selected_output_motifs[0].array[1] = 0;
    new_selected_output_motifs[0].array[2] = 1;
    new_selected_output_motifs[0].array[3] = 1;
    new_selected_output_motifs[0].array[4] = 3;
    new_selected_output_motifs[0].array[5] = 3;
    new_selected_output_motifs[0].array[6] = 3;


    new_selected_input_motifs[1].array[0] = 0;
    new_selected_input_motifs[1].array[1] = 2;
    new_selected_input_motifs[1].array[2] = 3;

    new_selected_output_motifs[1].array[0] = 2;


    // now add new motifs and connections and test
    NSGA2Type nsga2Params;
    add_motif_mutation_test(&nsga2Params, ind, n_new_motifs, new_selected_input_motifs, new_selected_output_motifs, 0);

    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));


    target_matrix[1] = 1;
    target_matrix[8] = 2;
    target_matrix[10] = 2;
    target_matrix[12] = 2;
    target_matrix[14] = 1;

    target_matrix[19] = 1;
    target_matrix[24] = 2;
    target_matrix[26] = 2;
    target_matrix[28] = 2;

    target_matrix[37] = 1;
    target_matrix[40] = 2;
    target_matrix[42] = 2;
    target_matrix[44] = 2;
    target_matrix[46] = 4;

    target_matrix[55] = 1;
    target_matrix[60] = 2;
    target_matrix[62] = 1;

    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);


# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // if not equals. return
    if(equals == 1){
        
        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 0 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->initial_global_index != 4 
            || motif_node->next_motif->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->next_motif->initial_global_index != 6 
            || motif_node->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 4 || ind->n_neurons != 8 || ind->n_synapses != 30){
            equals = 0;
        }

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons){
            equals = 0;        
        }
    }
    
    return equals;
}


void print_result(int result, int test_id){
    if(result == 1) printf("      Tests %d: SUCCESS!!\n", test_id, KGRN);
    else printf("      Tests %d: FAILURE!!\n", test_id, KRED);
}

void print_results(int *results, int n){
    int i;
    for( i=0; i<n; i++){
        if(results[i] == 1) printf("      Tests %d: SUCCESS!!\n", i, KGRN);
        else printf("      Tests %d: FAILURE!!\n", i, KRED);
    }
}

int main(int argc, char **argv){
    int results[4];
    
    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: add motif mutation\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    print_result(results[0], 0);
    
    results[1] = test2();
    print_result(results[1], 1);

    results[2] = test3();
    print_result(results[2], 2);
    
    results[3] = test4();
    print_result(results[3], 3);

    printf("\n");
}