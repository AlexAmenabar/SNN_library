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

    
    // initialize motifs
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
    

    // call function to test
    initialize_ind_test (ind, motif_types, selected_input_motifs, 4);


    // build target matrix
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

    // Now check that the dynamic lists of motifs and neurons are correct
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

        if(ind->n_motifs != 4 || ind->n_neurons != 11 || ind->n_synapses != 32)
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

// General test, with one doble connection between motifs
int test2(){
    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(3 * sizeof(int));

    
    // initialize motifs
    motif_types[0] = 0;
    motif_types[1] = 0;
    motif_types[2] = 0;
    
    selected_input_motifs = (int_array_t *)malloc(3 * sizeof(int_array_t));
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

    // call function to test
    initialize_ind_test (ind, motif_types, selected_input_motifs, 3);


    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

    target_matrix[1] = 1;

    target_matrix[6] = 2;
    target_matrix[8] = 2;
    target_matrix[10] = 2;

    target_matrix[15] = 1;

    target_matrix[18] = 2;
    target_matrix[20] = 2;
    target_matrix[22] = 2;

    target_matrix[29] = 1;

    target_matrix[30] = 2;
    target_matrix[32] = 2;
    target_matrix[34] = 2;


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

    // Now check that the dynamic lists of motifs and neurons are correct
    if(equals == 1){

        // check motifs
        motif_node = ind->motifs_new;
        if(motif_node->motif_type != 0 && motif_node->initial_global_index != 0
            || motif_node->next_motif->motif_type != 0 && motif_node->next_motif->initial_global_index != 2 
            || motif_node->next_motif->next_motif->motif_type != 0 && motif_node->next_motif->next_motif->initial_global_index != 4
            || motif_node->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 3 || ind->n_neurons != 6 || ind->n_synapses != 21)
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
    int results[5];

    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: initialize network\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    print_result(results[0], 0);

    results[1] = test2();
    print_result(results[1], 1);

    printf("\n");
}