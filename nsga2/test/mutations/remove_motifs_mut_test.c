#include "../test.h"

int test1(){
        
    /**
     * Aim: to test the initialization of all the struct of an individual.
     * - More than one connection between two motifs
     * - A motif connected with itself
     * 
     * Test description: 4 motifs (0, 2, 5, 6, 0, 4).
     * Connections:
     * - Number of input connections: [5, 1, 4, 4, 4, 4]
     * - Input connections -> [[1, 1, 3, 5, 5], [2], [0, 1, 3, 4], [2, 3, 3, 4], [0, 0, 2, 4], [3, 4, 5, 5]]
     * - Number of output connections: [3, 3, 3, 5, 4, 4]
     * - Output connections -> [[2, 4, 4], [0, 0, 2], [1, 3, 4], [0, 2, 3, 3, 5], [2, 3, 4, 5], [0, 0 ,5, 5]]
     */
    
    // define neccessary variables
    NSGA2Type *nsga2Params;
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs, *selected_output_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    // allocate memory for some variables
    nsga2Params = (NSGA2Type *)calloc(1, sizeof(NSGA2Type));
    ind = (individual *)calloc(1, sizeof(individual));
    motif_types = (int *)calloc(6, sizeof(int));

    // initialize input (synapses and neurons) paramter
    nsga2Params->image_size = 2;


    // initialize motifs
    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;
    motif_types[4] = 0;
    motif_types[5] = 4;
    
    selected_input_motifs = (int_array_t *)calloc(6, sizeof(int_array_t));
    selected_input_motifs[0].n = 5;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 4;
    selected_input_motifs[3].n = 4;
    selected_input_motifs[4].n = 4;
    selected_input_motifs[5].n = 4;

    selected_input_motifs[0].array = (int *)malloc(5 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[4].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[5].array = (int *)malloc(4 * sizeof(int));

    
    selected_output_motifs = (int_array_t *)calloc(6, sizeof(int_array_t));
    selected_output_motifs[0].n = 3;
    selected_output_motifs[1].n = 3;
    selected_output_motifs[2].n = 3;
    selected_output_motifs[3].n = 5;
    selected_output_motifs[4].n = 4;
    selected_output_motifs[5].n = 4;

    selected_output_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[1].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[3].array = (int *)malloc(5 * sizeof(int));
    selected_output_motifs[4].array = (int *)malloc(4 * sizeof(int));
    selected_output_motifs[5].array = (int *)malloc(4 * sizeof(int));

    // select input motifs for each motif
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;
    selected_input_motifs[0].array[3] = 5;
    selected_input_motifs[0].array[4] = 5;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;
    selected_input_motifs[2].array[3] = 4;

    selected_input_motifs[3].array[0] = 2;
    selected_input_motifs[3].array[1] = 3;
    selected_input_motifs[3].array[2] = 3;
    selected_input_motifs[3].array[3] = 4;

    selected_input_motifs[4].array[0] = 0;
    selected_input_motifs[4].array[1] = 0;
    selected_input_motifs[4].array[2] = 2;
    selected_input_motifs[4].array[3] = 4;

    selected_input_motifs[5].array[0] = 3;
    selected_input_motifs[5].array[1] = 4;
    selected_input_motifs[5].array[2] = 5;
    selected_input_motifs[5].array[3] = 5;

    // output    
    selected_output_motifs[0].array[0] = 2;
    selected_output_motifs[0].array[1] = 4;
    selected_output_motifs[0].array[2] = 4;

    selected_output_motifs[1].array[0] = 0;
    selected_output_motifs[1].array[1] = 0;
    selected_output_motifs[1].array[2] = 2;

    selected_output_motifs[2].array[0] = 1;
    selected_output_motifs[2].array[1] = 3;
    selected_output_motifs[2].array[2] = 4;

    selected_output_motifs[3].array[0] = 0;
    selected_output_motifs[3].array[1] = 2;
    selected_output_motifs[3].array[2] = 3;
    selected_output_motifs[3].array[3] = 3;
    selected_output_motifs[3].array[4] = 5;

    selected_output_motifs[4].array[0] = 2;
    selected_output_motifs[4].array[1] = 3;
    selected_output_motifs[4].array[2] = 4;
    selected_output_motifs[4].array[3] = 5;

    selected_output_motifs[5].array[0] = 0;
    selected_output_motifs[5].array[1] = 0;
    selected_output_motifs[5].array[2] = 5;
    selected_output_motifs[5].array[3] = 5;
    

    // initialize the individual
    initialize_ind_not_random (nsga2Params, ind, 6, motif_types, selected_input_motifs, selected_output_motifs);

    // mutate the individual
    int n_remove_motifs = 4;
    int_array_t *selected_motifs_to_remove = (int_array_t *)malloc(n_remove_motifs * sizeof(int_array_t));
    selected_motifs_to_remove->n = 4;
    selected_motifs_to_remove->array = (int *)malloc(n_remove_motifs * sizeof(int));
    selected_motifs_to_remove->array[0] = 0;
    selected_motifs_to_remove->array[1] = 1;
    selected_motifs_to_remove->array[2] = 4;
    selected_motifs_to_remove->array[3] = 5;

    printf(" Removing motifs!\n");
    fflush(stdout);
    remove_selected_motifs_mutation(nsga2Params, ind, selected_motifs_to_remove);
    printf(" Motifs removed!\n");
    fflush(stdout);

    // sparse matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    /*target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

    target_matrix[1] = 1;
    target_matrix[22] = 1;
    target_matrix[24] = 1;
    target_matrix[28] = 2;

    target_matrix[37] = 1;
    target_matrix[55] = 1;
    target_matrix[68] = 2;
    target_matrix[70] = -1;
    target_matrix[73] = 1;
    target_matrix[75] = 1;

    target_matrix[87] = 1;
    target_matrix[91] = 1;
    target_matrix[93] = 1;
    target_matrix[94] = 1;
    target_matrix[95] = 1;
    target_matrix[96] = 1;
    target_matrix[109] = -1;
    target_matrix[121] = 1;
    target_matrix[127] = 1;
    target_matrix[128] = 1;
    target_matrix[129] = 1;
    target_matrix[130] = 1;

    target_matrix[136] = 1;
    target_matrix[141] = 1;
    target_matrix[143] = 1;
    target_matrix[144] = 2;
    target_matrix[145] = 3;
    target_matrix[146] = 3;
    target_matrix[149] = 1;
    target_matrix[150] = 1;
    target_matrix[151] = 1;
    target_matrix[152] = 1;
    target_matrix[153] = 1;
    target_matrix[158] = 1;
    target_matrix[160] = 1;
    target_matrix[161] = 3;
    target_matrix[162] = 2;
    target_matrix[163] = 3;
    target_matrix[166] = 1;
    target_matrix[167] = 1;
    target_matrix[168] = 1;
    target_matrix[169] = 1;
    target_matrix[170] = 1;
    target_matrix[175] = 1;
    target_matrix[177] = 1;
    target_matrix[178] = 3;
    target_matrix[179] = 3;
    target_matrix[180] = 2;
    target_matrix[183] = 1;
    target_matrix[184] = 1;
    target_matrix[185] = 1;
    target_matrix[186] = 1;

    target_matrix[199] = 1;
    target_matrix[209] = 1;
    target_matrix[211] = 1;
    target_matrix[212] = 1;
    target_matrix[213] = 1;
    target_matrix[214] = 1;
    target_matrix[215] = 1;
    target_matrix[217] = 1;
    target_matrix[218] = 1;
    target_matrix[219] = 1;
    target_matrix[220] = 1;

    target_matrix[221] = 2;
    target_matrix[234] = 2;
    target_matrix[235] = -3;
    target_matrix[236] = 2;
    target_matrix[237] = 2;
    target_matrix[238] = 2;
    target_matrix[251] = 2;
    target_matrix[252] = 2;
    target_matrix[253] = -3;
    target_matrix[254] = 2;
    target_matrix[255] = 2;
    target_matrix[268] = 2;
    target_matrix[269] = 2;
    target_matrix[270] = 2;
    target_matrix[271] = -3;
    target_matrix[272] = 2;
    target_matrix[285] = -3;
    target_matrix[286] = 2;
    target_matrix[287] = 2;
    target_matrix[288] = 2;*/

    // get sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare if the sparse matrix is correctly constructed
    //equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    //print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

    new_motif_t *motif_node;
    neuron_node_t *neuron_node;
    int_node_t *int_node;
    
    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check if values in the linked list of motifs are correct
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 2 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 5 && motif_node->next_motif->next_motif->initial_global_index != 5
            || motif_node->next_motif->next_motif->next_motif->motif_type != 6 && motif_node->next_motif->next_motif->next_motif->initial_global_index != 8 
            || motif_node->next_motif->next_motif->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->next_motif->next_motif->initial_global_index != 11
            || motif_node->next_motif->next_motif->next_motif->next_motif->next_motif->motif_type != 4 && motif_node->next_motif->next_motif->next_motif->next_motif->next_motif->initial_global_index != 13
            || motif_node->next_motif->next_motif->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }


        if(ind->n_motifs != 6 || ind->n_neurons != 17 || ind->n_synapses - ind->n_input_synapses != 124)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;


        // check linked lists of connections
        if(ind->connectivity_info.in_connections[0].n_nodes != 5 || ind->connectivity_info.in_connections[1].n_nodes != 1 ||
            ind->connectivity_info.in_connections[2].n_nodes != 4 || ind->connectivity_info.in_connections[3].n_nodes != 4 ||
            ind->connectivity_info.in_connections[4].n_nodes != 4 || ind->connectivity_info.in_connections[5].n_nodes != 4)
            equals = 0;

        if(ind->connectivity_info.out_connections[0].n_nodes != 3 || ind->connectivity_info.out_connections[1].n_nodes != 3 ||
            ind->connectivity_info.out_connections[2].n_nodes != 3 || ind->connectivity_info.out_connections[3].n_nodes != 5 ||
            ind->connectivity_info.out_connections[4].n_nodes != 4 || ind->connectivity_info.out_connections[5].n_nodes != 4)
            equals = 0;


        if(ind->connectivity_info.in_connections[0].first_node->value != 1 || ind->connectivity_info.in_connections[0].first_node->next->value != 1 ||
            ind->connectivity_info.in_connections[0].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[0].first_node->next->next->next->value != 5 ||
            ind->connectivity_info.in_connections[0].first_node->next->next->next->next->value != 5 || 
            ind->connectivity_info.in_connections[0].first_node->next->next->next->next->next != NULL ||
            ind->connectivity_info.in_connections[1].first_node->value != 2 || ind->connectivity_info.in_connections[1].first_node->next != NULL ||
            ind->connectivity_info.in_connections[2].first_node->value != 0 || ind->connectivity_info.in_connections[2].first_node->next->value != 1 ||
            ind->connectivity_info.in_connections[2].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[2].first_node->next->next->next->value != 4 ||
            ind->connectivity_info.in_connections[2].first_node->next->next->next->next != NULL ||
            ind->connectivity_info.in_connections[3].first_node->value != 2 || ind->connectivity_info.in_connections[3].first_node->next->value != 3 ||
            ind->connectivity_info.in_connections[3].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[3].first_node->next->next->next->value != 4 || 
            ind->connectivity_info.in_connections[3].first_node->next->next->next->next != NULL ||
            ind->connectivity_info.in_connections[4].first_node->value != 0 || ind->connectivity_info.in_connections[4].first_node->next->value != 0 ||
            ind->connectivity_info.in_connections[4].first_node->next->next->value != 2 || ind->connectivity_info.in_connections[4].first_node->next->next->next->value != 4 ||
            ind->connectivity_info.in_connections[4].first_node->next->next->next->next != 0 ||
            ind->connectivity_info.in_connections[5].first_node->value != 3 || ind->connectivity_info.in_connections[5].first_node->next->value != 4 || 
            ind->connectivity_info.in_connections[5].first_node->next->next->value != 5 || ind->connectivity_info.in_connections[5].first_node->next->next->next->value != 5 ||
            ind->connectivity_info.in_connections[5].first_node->next->next->next->next != NULL)
           equals = 0;

        if(ind->connectivity_info.out_connections[0].first_node->value != 2 || ind->connectivity_info.out_connections[0].first_node->next->value != 4 ||
            ind->connectivity_info.out_connections[0].first_node->next->next->value != 4 || ind->connectivity_info.out_connections[0].first_node->next->next->next != NULL || 
            ind->connectivity_info.out_connections[1].first_node->value != 0 || ind->connectivity_info.out_connections[1].first_node->next->value != 0 || 
            ind->connectivity_info.out_connections[1].first_node->next->next->value != 2 || ind->connectivity_info.out_connections[1].first_node->next->next->next != NULL || 
            ind->connectivity_info.out_connections[2].first_node->value != 1 || ind->connectivity_info.out_connections[2].first_node->next->value != 3 || 
            ind->connectivity_info.out_connections[2].first_node->next->next->value != 4 || ind->connectivity_info.out_connections[2].first_node->next->next->next != NULL ||
            ind->connectivity_info.out_connections[3].first_node->value != 0 || ind->connectivity_info.out_connections[3].first_node->next->value != 2 ||
            ind->connectivity_info.out_connections[3].first_node->next->next->value != 3 || ind->connectivity_info.out_connections[3].first_node->next->next->next->value != 3 ||
            ind->connectivity_info.out_connections[3].first_node->next->next->next->next->value != 5 || ind->connectivity_info.out_connections[3].first_node->next->next->next->next->next != NULL ||
            ind->connectivity_info.out_connections[4].first_node->value != 2 || ind->connectivity_info.out_connections[4].first_node->next->value != 3 ||
            ind->connectivity_info.out_connections[4].first_node->next->next->value != 4 || ind->connectivity_info.out_connections[4].first_node->next->next->next->value != 5 ||
            ind->connectivity_info.out_connections[4].first_node->next->next->next->next != NULL ||
            ind->connectivity_info.out_connections[5].first_node->value != 0 || ind->connectivity_info.out_connections[5].first_node->next->value != 0 ||
            ind->connectivity_info.out_connections[5].first_node->next->next->value != 5 ||
            ind->connectivity_info.out_connections[5].first_node->next->next->next->value != 5 ||
            ind->connectivity_info.out_connections[5].first_node->next->next->next->next != NULL)
           equals = 0;
    }


    return equals;
}


// General test, with one doble connection between motifs
/*int test1(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 4;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(4 * sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 1;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(1 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 1;


    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(6 * 6, sizeof(int));
    result_matrix = (int *)calloc(6 * 6, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[8] = -1;

    target_matrix[18] = 1;
    target_matrix[20] = 1;
    target_matrix[22] = 1;
    target_matrix[23] = 1;

    target_matrix[24] = 1;
    target_matrix[26] = 1;
    target_matrix[27] = 1;
    target_matrix[29] = 1;

    target_matrix[30] = 1;
    target_matrix[32] = 1;
    target_matrix[33] = 1;
    target_matrix[34] = 1;


    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 2;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 0;
    motifs_to_remove->array[1] = 1;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 2, motifs_to_remove);

    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 5 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 6 && motif_node->next_motif->initial_global_index != 3 
            || motif_node->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 2 || ind->n_neurons != 6 || ind->n_synapses != 14)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}

int test2(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 4;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(4 * sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 1;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(1 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 1;

    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(8 * 8, sizeof(int));
    result_matrix = (int *)calloc(8 * 8, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[13] = 1;
    target_matrix[15] = 1;

    target_matrix[19] = 1;
    
    target_matrix[28] = 1;

    target_matrix[32] = 2;
    target_matrix[34] = -1;
    target_matrix[37] = 1;
    target_matrix[39] = 1;

    target_matrix[42] = 1;
    target_matrix[46] = 1;

    target_matrix[55] = -1;

    target_matrix[58] = 1;

    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 1;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 3;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 1, motifs_to_remove);

    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 2 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 5 && motif_node->next_motif->next_motif->initial_global_index != 5 
            || motif_node->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 3 || ind->n_neurons != 8 || ind->n_synapses != 14)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}

int test3(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 4;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(4 * sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 1;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(1 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 1;

    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(5 * 5, sizeof(int));
    result_matrix = (int *)calloc(5 * 5, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[10] = 1;
    target_matrix[13] = 1;
    target_matrix[14] = 1;

    target_matrix[15] = 1;
    target_matrix[17] = 1;
    target_matrix[19] = 1;
    
    target_matrix[20] = 1;
    target_matrix[22] = 1;
    target_matrix[23] = 1;


    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 2;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 1;
    motifs_to_remove->array[1] = 2;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 2, motifs_to_remove);

    ind->n_neurons = 5;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 6 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 2 || ind->n_neurons != 5 || ind->n_synapses != 10)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}

int test4(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 4;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(4 * sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 1;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(1 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 1;

    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(5 * 5, sizeof(int));
    result_matrix = (int *)calloc(5 * 5, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[13] = 1;

    target_matrix[19] = 1;

    target_matrix[20] = 2;
    target_matrix[22] = -1;


    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 2;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 2;
    motifs_to_remove->array[1] = 3;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 2, motifs_to_remove);


    ind->n_neurons = 5;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 2 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 2 || ind->n_neurons != 5 || ind->n_synapses != 6)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}

int test5(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 4;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;

    selected_input_motifs = (int_array_t *)malloc(4 * sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 1;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(1 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 1;

    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(6 * 6, sizeof(int));
    result_matrix = (int *)calloc(6 * 6, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[8] = 1;

    target_matrix[12] = -1;
    target_matrix[15] = 1;
    target_matrix[16] = 1;
    target_matrix[17] = 1;

    target_matrix[22] = 1;
    target_matrix[23] = 1;

    target_matrix[27] = 1;
    target_matrix[29] = 1;

    target_matrix[33] = 1;
    target_matrix[34] = 1;


    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 2;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 0;
    motifs_to_remove->array[1] = 2;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 2, motifs_to_remove);


    ind->n_neurons = 6;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 2 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 6 && motif_node->next_motif->initial_global_index != 3 
            || motif_node->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 2 || ind->n_neurons != 6 || ind->n_synapses != 12)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}


int test6(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(4 * sizeof(int));

    // configure test
    ind->n_motifs = 5;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 1;
    motif_types[1] = 4;
    motif_types[2] = 2;
    motif_types[3] = 0;
    motif_types[4] = 0;

    selected_input_motifs = (int_array_t *)malloc(5 * sizeof(int_array_t));
    selected_input_motifs[0].n = 4;
    selected_input_motifs[1].n = 3;
    selected_input_motifs[2].n = 5;
    selected_input_motifs[3].n = 5;
    selected_input_motifs[4].n = 2;

    selected_input_motifs[0].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(6 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(5 * sizeof(int));
    selected_input_motifs[4].array = (int *)malloc(2 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;
    selected_input_motifs[0].array[2] = 2;
    selected_input_motifs[0].array[3] = 4;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 1;
    selected_input_motifs[1].array[2] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 3;
    selected_input_motifs[2].array[2] = 3;
    selected_input_motifs[2].array[3] = 3;
    selected_input_motifs[2].array[4] = 3;

    selected_input_motifs[3].array[0] = 0;
    selected_input_motifs[3].array[1] = 0;
    selected_input_motifs[3].array[2] = 0;
    selected_input_motifs[3].array[3] = 1;
    selected_input_motifs[3].array[4] = 3;

    selected_input_motifs[4].array[0] = 0;
    selected_input_motifs[4].array[1] = 1;
    

    // initialize ind motifs from types
    initialize_ind_motifs_from_types(ind, motif_types);

    // initialize neurons (depend on motifs)
    initialize_neuron_nodes(ind);
    
    // Connect motifs and its neurons: connect each motif to its first neuron in the neuron list
    connect_motifs_and_neurons(ind);
    
    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);


    // allocate memory to store expected and obtained matrix
    target_matrix = (int *)calloc(13 * 13, sizeof(int));
    result_matrix = (int *)calloc(13 * 13, sizeof(int));

    // construct expected matrix
    target_matrix[1] = 1;

    target_matrix[8] = 1;

    target_matrix[12] = -1;
    target_matrix[15] = 1;
    target_matrix[16] = 1;
    target_matrix[17] = 1;

    target_matrix[22] = 1;
    target_matrix[23] = 1;

    target_matrix[27] = 1;
    target_matrix[29] = 1;

    target_matrix[33] = 1;
    target_matrix[34] = 1;


//# ifdef DEBUG
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    print_synapses_dynamic_list(ind->connectivity_matrix);
    //print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
//# endif

    // prepare data
    int_array_t *motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    motifs_to_remove->n = 3;
    motifs_to_remove->array = (int *)malloc(motifs_to_remove->n * sizeof(int));
    motifs_to_remove->array[0] = 0;
    motifs_to_remove->array[1] = 1;
    motifs_to_remove->array[2] = 3;


    // remove motifs from sparse matrix
    remove_motif_mutation_test(ind, 2, motifs_to_remove);


    ind->n_neurons = 5;
    free(result_matrix);
    result_matrix = (int *)calloc(5 * 5, sizeof(int));
    
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

//# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    //print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
//# endif


    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 2 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 6 && motif_node->next_motif->initial_global_index != 3 
            || motif_node->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 2 || ind->n_neurons != 6 || ind->n_synapses != 12)
            equals = 0;

        // count neurons too
        int n_neurons = 0;
        neuron_node = ind->neurons;
        while(neuron_node){
            n_neurons ++;
            neuron_node = neuron_node->next_neuron;
        }

        if(ind->n_neurons != n_neurons)
            equals = 0;
    }

    return equals;
}*/

int main(int argc, char **argv){
    int results[6];

    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: remove motif(s) mutation\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    print_result(results[0], 0);

/*   results[1] = test2();
    print_result(results[1], 1);
    
    results[2] = test3();
    print_result(results[2], 2);
    
    results[3] = test4();
    print_result(results[3], 3);
    
    results[4] = test5();
    print_result(results[4], 4);

    results[5] = test6();
    print_result(results[5], 5);*/

    printf("\n");
}