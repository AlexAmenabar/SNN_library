# include "nsga2.h"
# include "snn_library.h"

# define KRED "\x1B[31m"
# define KGRN "\x1B[32m"


void remove_motif_mutation_test(individual *ind, int n_remove_motifs, int_array_t *selected_motifs_to_remove){
    int i, j, s;
    new_motif_t *previous_motif_node, *tmp_motif_node, *motif_node;
    neuron_node_t *previous_neuron_node, *neuron_node, *tmp_neuron_node;
    motif_t *motif_data;
    int rest_neurons = 0;

    // update control values of the SNN

    // update sparse matrix removing neccessary synapses
    update_sparse_matrix_remove_motifs(ind, selected_motifs_to_remove);
    ind->n_motifs -= n_remove_motifs;

    // remove deleted motifs, and for each motif, its neurons, from the dynamic lists
    motif_node = ind->motifs_new;
    neuron_node = ind->neurons;

    i = 0;
    j = 0; 
    
    // loop until selected motifs are removed from the list
    while(j < selected_motifs_to_remove->n){

        tmp_motif_node = motif_node;
        tmp_motif_node->initial_global_index -= rest_neurons;
        motif_data = &(motifs_data[tmp_motif_node->motif_type]);

        motif_node = motif_node->next_motif;

        // check if the actual motif must be removed
        if(i == selected_motifs_to_remove->array[j]){

            // motif will be removed, so update 
            rest_neurons += motif_data->n_neurons;
            ind->n_neurons -= motif_data->n_neurons;

            // if the tmp motif is the first one, change the first motif of the list
            if(tmp_motif_node == ind->motifs_new){
                ind->motifs_new = motif_node;
            }
            // else, remove and connect previous with next one
            else{
                previous_motif_node->next_motif = motif_node;
            }
            
            // free memory of the motif node
            free(tmp_motif_node);

            // loop over motif neurons, remove them and connect the previous one with the first of the next motif
            for(s = 0; s<motif_data->n_neurons; s++){
                tmp_neuron_node = neuron_node;
                neuron_node = neuron_node->next_neuron;

                if(tmp_neuron_node == ind->neurons)
                    ind->neurons = neuron_node;
                else
                    previous_neuron_node->next_neuron = neuron_node;

                free(tmp_neuron_node);
            }


            // move to the next motif to be removed
            j++;
        }
        // update only previous motif node
        else{
            previous_motif_node = tmp_motif_node;

            // loop over motif neurons
            for(s = 0; s<motif_data->n_neurons; s++){
                previous_neuron_node = neuron_node;
                neuron_node = neuron_node->next_neuron;
            }
        }

        i++;
    }
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
}


void print_result(int result, int test_id){
    if(result == 1) printf("      Tests %d: SUCCESS!!\n", test_id);
    else printf("      Tests %d: FAILURE!!\n", test_id);
}

void print_results(int *results, int n){
    int i;
    for( i=0; i<n; i++){
        if(results[i] == 1) printf("      Tests %d: SUCCESS!!\n", i, KGRN);
        else printf("      Tests %d: FAILURE!!\n", i, KRED);
    }
}

int main(int argc, char **argv){
    int results[6];

    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: remove motif(s) mutation\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    print_result(results[0], 0);

    results[1] = test2();
    print_result(results[1], 1);
    
    results[2] = test3();
    print_result(results[2], 2);
    
    results[3] = test4();
    print_result(results[3], 3);
    
    results[4] = test5();
    print_result(results[4], 4);

    results[5] = test6();
    print_result(results[5], 5);

    printf("\n");
}