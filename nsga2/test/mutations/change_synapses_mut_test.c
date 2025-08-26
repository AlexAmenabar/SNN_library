#include "../test.h"
#include "../../rand.h"

void synapse_change_mutation_test(NSGA2Type *nsga2Params, individual *ind, int mutation_code, int_array_t *selected_synapses){
    // select neurons to change
    int i, j = 0, s = 0, mutation_parameter;

    // select what property should be changed
    /*if(nsga2Params->weights_included == 1)
        mutation_parameter = rnd(0, 1); // the code indicates what mutation will be done
    else
        mutation_parameter = 0;*/
    mutation_parameter = 0;

    
    // loop over dynamic list of neurons to find the selected ones and change them
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    s = 0; // index of the synapse inside in the synapse node


    printf(" > > n_synapses = %d, n_input_synapses = %d, selected synapses: ", ind->n_synapses, ind->n_input_synapses);
    for(i = 0; i<selected_synapses->n; i++){
        printf("%d,", selected_synapses->array[i]);
    }
    printf("\n");

    int counter = 0;
    while(synapse_node){
        counter += abs(synapse_node->value);
        synapse_node = synapse_node->next_element;
    }
    printf(" > > Counter %d\n", counter);

    synapse_node = ind->connectivity_matrix;

    // loop over synapses to find those that must be mutated
    for(i = 0; i<selected_synapses->n; i++){
        printf(" > > > i = %d, j = %d, s = %d, selected = %d\n", i, j, s, selected_synapses->array[i]);
        // loop until we reach the selected neuron node
        while(j != selected_synapses->array[i]){

            // each node has several synapses, so loop over those
            if(s == abs(synapse_node->value) - 1){
                s = 0;
                synapse_node = synapse_node->next_element;
            }
            else{
                s++;
            }
            
            j++;
        }

        switch(mutation_parameter){
            case 0:
                synapse_node->latency[s] = synapse_node->latency[s] + (rnd(-2, 2));
                if(synapse_node->latency[s] <= 0)
                    synapse_node->latency[s] = 1;
                break;
            case 1:
                synapse_node->weight[s] = synapse_node->weight[s] + (synapse_node->weight[s] * (rndreal(-0.5, 0.5)));
                break;
            case 2:
                synapse_node->learning_rule[s] = synapse_node->learning_rule[s]; // TODO
                break;
        }
    }
}



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
    motif_types = (int *)calloc(3, sizeof(int));

    // initialize input (synapses and neurons) paramter
    nsga2Params->image_size = 2;


    // initialize motifs
    motif_types[0] = 0;
    motif_types[1] = 0;
    motif_types[2] = 0;
    
    selected_input_motifs = (int_array_t *)calloc(3, sizeof(int_array_t));
    selected_input_motifs[0].n = 2;
    selected_input_motifs[1].n = 5;
    selected_input_motifs[2].n = 4;

    selected_input_motifs[0].array = (int *)malloc(2 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(5 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(4 * sizeof(int));
    
    selected_output_motifs = (int_array_t *)calloc(3, sizeof(int_array_t));
    selected_output_motifs[0].n = 1;
    selected_output_motifs[1].n = 2;
    selected_output_motifs[2].n = 2;

    selected_output_motifs[0].array = (int *)malloc(1 * sizeof(int));
    selected_output_motifs[1].array = (int *)malloc(2 * sizeof(int));
    selected_output_motifs[2].array = (int *)malloc(2 * sizeof(int));

    // select input motifs for each motif
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 0;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 1;
    selected_input_motifs[1].array[2] = 2;
    selected_input_motifs[1].array[3] = 2;
    selected_input_motifs[1].array[4] = 2;

    selected_input_motifs[2].array[0] = 0;
    selected_input_motifs[2].array[1] = 0;
    selected_input_motifs[2].array[2] = 1;
    selected_input_motifs[2].array[3] = 2;


    // output    
    selected_output_motifs[0].array[0] = 1;

    selected_output_motifs[1].array[0] = 1;
    selected_output_motifs[1].array[1] = 2;

    selected_output_motifs[2].array[0] = 1;
    selected_output_motifs[2].array[1] = 1;


    // initialize the individual
    initialize_ind_not_random (nsga2Params, ind, 3, motif_types, selected_input_motifs, selected_output_motifs);


    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;
    while(synapse_node){
        printf(" > > > row = %d, col = %d, value = %d\n", synapse_node->row, synapse_node->col, synapse_node->value);
        synapse_node = synapse_node->next_element;
    }

    // mutate the individual
    int_array_t *selected_synapses = (int_array_t *)malloc(sizeof(int_array_t));
    selected_synapses->n = 17;
    selected_synapses->array = (int *)malloc(17 * sizeof(int));
    selected_synapses->array[0] = 0;
    selected_synapses->array[1] = 0;
    selected_synapses->array[2] = 1;
    selected_synapses->array[3] = 2;
    selected_synapses->array[4] = 3;
    selected_synapses->array[5] = 4;
    selected_synapses->array[6] = 5;
    selected_synapses->array[7] = 6;
    selected_synapses->array[8] = 7;
    selected_synapses->array[9] = 7;
    selected_synapses->array[10] = 8;
    selected_synapses->array[11] = 9;
    selected_synapses->array[12] = 10;
    selected_synapses->array[13] = 11;
    selected_synapses->array[14] = 12;
    selected_synapses->array[15] = 13;
    selected_synapses->array[16] = 13;

    synapse_change_mutation_test(nsga2Params, ind, 0, selected_synapses);

    return 1;
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