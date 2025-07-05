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


    // build the sparse matrix
    new_construct_sparse_matrix(ind, selected_input_motifs);

#ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
#endif

    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

# ifdef DEBUG
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
# endif

    // free

    return equals;
}

int test2(){

    int i, j, equals;
    int *motif_types;
    int_array_t *selected_input_motifs;
    individual *ind;
    int *result_matrix, *target_matrix;

    ind = (individual *)malloc(sizeof(individual));
    motif_types = (int *)malloc(3 * sizeof(int));

    // configure test
    ind->n_motifs = 3;
    ind->n_neurons = 0;
    ind->n_synapses = 0;

    motif_types[0] = 3;
    motif_types[1] = 4;
    motif_types[2] = 2;

    selected_input_motifs = (int_array_t *)malloc(3 * sizeof(int_array_t));
    selected_input_motifs[0].n = 4;
    selected_input_motifs[1].n = 2;
    selected_input_motifs[2].n = 2;

    selected_input_motifs[0].array = (int *)malloc(4 * sizeof(int));
    selected_input_motifs[1].array = (int *)malloc(2 * sizeof(int));
    selected_input_motifs[2].array = (int *)malloc(2 * sizeof(int));
    
    // select input motifs
    selected_input_motifs[0].array[0] = 0;
    selected_input_motifs[0].array[1] = 1;
    selected_input_motifs[0].array[2] = 2;
    selected_input_motifs[0].array[3] = 2;

    selected_input_motifs[1].array[0] = 0;
    selected_input_motifs[1].array[1] = 2;

    selected_input_motifs[2].array[0] = 2;
    selected_input_motifs[2].array[1] = 2;

    // initialize motifs dynamic list
    initialize_ind_motifs_from_types(ind, motif_types);

    // build target matrix
    result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
    target_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));

    target_matrix[0] = 1;
    target_matrix[1] = 2;
    target_matrix[2] = 1;
    target_matrix[3] = 1;
    target_matrix[4] = 1;
    target_matrix[5] = 1;
    target_matrix[6] = 1;
    target_matrix[7] = 1;

    target_matrix[11] = 1;
    target_matrix[12] = 1;
    target_matrix[13] = 2;
    target_matrix[14] = 1;
    target_matrix[15] = 1;
    target_matrix[16] = 1;
    target_matrix[17] = 1;
    target_matrix[18] = 1;

    target_matrix[22] = 1;
    target_matrix[23] = 1;
    target_matrix[24] = 1;
    target_matrix[25] = 2;
    target_matrix[26] = 1;
    target_matrix[27] = 1;
    target_matrix[28] = 1;
    target_matrix[29] = 1;

    target_matrix[33] = 2;
    target_matrix[34] = 1;
    target_matrix[35] = 1;
    target_matrix[36] = 1;
    target_matrix[37] = 1;
    target_matrix[38] = 1;
    target_matrix[39] = 1;
    target_matrix[40] = 1;


    target_matrix[44] = 1;
    target_matrix[45] = 1;
    target_matrix[46] = 1;
    target_matrix[47] = 1;
    target_matrix[49] = -1;

    target_matrix[55] = 1;
    target_matrix[56] = 1;
    target_matrix[57] = 1;
    target_matrix[58] = 1;
    target_matrix[61] = -1;

    target_matrix[66] = 1;
    target_matrix[67] = 1;
    target_matrix[68] = 1;
    target_matrix[69] = 1;
    target_matrix[73] = -1;
    
    target_matrix[77] = 1;
    target_matrix[78] = 1;
    target_matrix[79] = 1;
    target_matrix[80] = 1;
    target_matrix[81] = -1;


    target_matrix[97] = 1;
    target_matrix[109] = 1;
    target_matrix[110] = 2;
    target_matrix[111] = 2;
    target_matrix[112] = 2;
    target_matrix[113] = 2;

    target_matrix[114] = 1;
    target_matrix[115] = 1;
    target_matrix[116] = 1;
    target_matrix[117] = 1;

    target_matrix[118] = -3;

    // build the sparse matrix
    new_construct_sparse_matrix(ind,  selected_input_motifs);

#ifdef DEBUG
    print_synapses_dynamic_list(ind->connectivity_matrix);
#endif

    // build complete matrix from the obtained sparse matrix
    get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

    // compare obtained matrix and the expected one
    equals = compare_target_and_obtained_matrix(result_matrix, target_matrix, ind->n_neurons);

#ifdef DEBUG
    print_connectivity_matrix(target_matrix, ind->n_neurons);
    print_connectivity_matrix(result_matrix, ind->n_neurons);
#endif

    return equals;
}

int test3(){

}

int test4(){

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

    printf("   = == ====== == ====== == =   \nStarting tests: build sparse matrix\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    
    results[1] = test2();

    //results[2] = test3();
    
    //results[3] = test4();

    print_result(results[0], 0);
    print_result(results[1], 1);
    //print_result(results[2], 2);
    //print_result(results[3], 3);

    printf("\n");
}