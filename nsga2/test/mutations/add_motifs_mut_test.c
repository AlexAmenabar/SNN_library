# include "../test.h"


int test1(){
        
    /**
     * Aim: to test the initialization of all the struct of an individual.
     * - More than one connection between two motifs
     * - A motif connected with itself
     * 
     * Test description: 4 motifs (0, 2, 5, 6).
     * Connections:
     * - Number of input connections: [3, 1, 3, 3]
     * - Input connections -> [[1, 1, 3], [2], [0, 1, 3], [2, 3, 3]]
     * - Number of output connections: [1, 3, 2, 4]
     * - Output connections -> [[2], [0, 0, 2], [1, 3], [0, 2, 3, 3]]
     * 
     * 
     * Mutation: add two motifs -> [0, 4]
     * Input connections of the motifs: [0, 0, 2], [3, 4, 5]
     * Output connection of the motifs: [2, 3, 4], [0, 0, 5]
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
    motif_types = (int *)calloc(4, sizeof(int));

    // initialize input (synapses and neurons) paramter
    nsga2Params->image_size = 2;


    // initialize motifs
    motif_types[0] = 0;
    motif_types[1] = 2;
    motif_types[2] = 5;
    motif_types[3] = 6;
    
    selected_input_motifs = (int_array_t *)calloc(4, sizeof(int_array_t));
    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 1;
    selected_input_motifs[2].n = 3;
    selected_input_motifs[3].n = 3;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(1 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[3].array = (int *)malloc(3 * sizeof(int));
    
    selected_output_motifs = (int_array_t *)calloc(4, sizeof(int_array_t));
    selected_output_motifs[0].n = 1;
    selected_output_motifs[1].n = 3;
    selected_output_motifs[2].n = 2;
    selected_output_motifs[3].n = 4;

    selected_output_motifs[0].array = (int *)malloc(1 * sizeof(int));
    selected_output_motifs[1].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[2].array = (int *)malloc(2 * sizeof(int));
    selected_output_motifs[3].array = (int *)malloc(4 * sizeof(int));
    

    // select input motifs for each motif
    selected_input_motifs[0].array[0] = 1;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 3;

    selected_input_motifs[1].array[0] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 1;
    selected_input_motifs[2].array[2] = 3;

    selected_input_motifs[3].array[0] = 2;
    selected_input_motifs[3].array[1] = 3;
    selected_input_motifs[3].array[2] = 3;

    
    selected_output_motifs[0].array[0] = 2;

    selected_output_motifs[1].array[0] = 0;
    selected_output_motifs[1].array[1] = 0;
    selected_output_motifs[1].array[2] = 2;

    selected_output_motifs[2].array[0] = 1;
    selected_output_motifs[2].array[1] = 3;

    selected_output_motifs[3].array[0] = 0;
    selected_output_motifs[3].array[1] = 2;
    selected_output_motifs[3].array[2] = 3;
    selected_output_motifs[3].array[3] = 3;
    

    // initialize the individual
    initialize_ind_not_random (nsga2Params, ind, 4, motif_types, selected_input_motifs, selected_output_motifs);

    // mutate the individual
    int n_new_motifs = 2;
    int *new_motifs;
    
    // initialize new motif types
    new_motifs = (int *)malloc(n_new_motifs * sizeof(int));
    new_motifs[0] = 0;
    new_motifs[1] = 4;

    // selected new input and output motifs
    deallocate_int_arrays(selected_input_motifs, 4);
    deallocate_int_arrays(selected_output_motifs, 4);
    selected_input_motifs = (int_array_t *)malloc(2 * sizeof(int_array_t));
    selected_output_motifs = (int_array_t *)malloc(2 * sizeof(int_array_t));

    selected_input_motifs[0].n = 3;
    selected_input_motifs[1].n = 3;
    selected_output_motifs[0].n = 3;
    selected_output_motifs[1].n = 3;

    selected_input_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[0].array = (int *)malloc(3 * sizeof(int));
    selected_output_motifs[1].array = (int *)malloc(3 * sizeof(int));

    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;
    selected_input_motifs[0].array[2] = 2;

    selected_output_motifs[0].array[0] = 2;
    selected_output_motifs[0].array[1] = 3;
    selected_output_motifs[0].array[2] = 4;

    selected_input_motifs[1].array[0] = 3;
    selected_input_motifs[1].array[1] = 4;
    selected_input_motifs[1].array[2] = 5;

    selected_output_motifs[1].array[0] = 0;
    selected_output_motifs[1].array[1] = 0;
    selected_output_motifs[1].array[2] = 5;


    // perform the mutation
    add_motif_mutation(nsga2Params, ind, n_new_motifs, new_motifs, selected_input_motifs, selected_output_motifs);


    // sparse matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

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
    target_matrix[288] = 2;

    // get sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare if the sparse matrix is correctly constructed
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
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
/*int test5(){
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
    //target_matrix[52] = 1;
    //target_matrix[53] = 1;
    //target_matrix[54] = 1;

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
/*    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

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
}*/

int main(int argc, char **argv){
    int results[4];
    
    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: add motif mutation\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    print_result(results[0], 0);
    
    /*results[1] = test2();
    print_result(results[1], 1);

    results[2] = test3();
    print_result(results[2], 2);
    
    results[3] = test4();
    print_result(results[3], 3);*/

    printf("\n");
}