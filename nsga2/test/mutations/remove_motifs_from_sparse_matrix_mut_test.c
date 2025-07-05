# include "nsga2.h"
# include "snn_library.h"

# define KRED "\x1B[31m"
# define KGRN "\x1B[32m"


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

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

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
    update_sparse_matrix_remove_motifs(ind, motifs_to_remove);


    ind->n_neurons = 6;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

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

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

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
    update_sparse_matrix_remove_motifs(ind, motifs_to_remove);

    ind->n_neurons = 8;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

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

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

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
    update_sparse_matrix_remove_motifs(ind, motifs_to_remove);

    ind->n_neurons = 5;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

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

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

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
    update_sparse_matrix_remove_motifs(ind, motifs_to_remove);


    ind->n_neurons = 5;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

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

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

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
    update_sparse_matrix_remove_motifs(ind, motifs_to_remove);


    ind->n_neurons = 6;
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);
    

# ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

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

    printf("   = == ====== == ====== == =   \nStarting tests: update (remove) sparse matrix\n   = == ====== == ====== == =   \n");

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
    
    printf("\n");
}