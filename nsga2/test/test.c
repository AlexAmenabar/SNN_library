#include "test.h"


/* Function to print the results of the tests */

// print the result of only one test
void print_result(int result, int test_id){
    if(result == 1) printf("      Tests %d: SUCCESS!!\n", test_id, KGRN);
    else printf("      Tests %d: FAILURE!!\n", test_id, KRED);
}

// print the results of several tests
void print_results(int *results, int n){
    int i;
    for( i=0; i<n; i++){
        if(results[i] == 1) printf("      Tests %d: SUCCESS!!\n", i, KGRN);
        else printf("      Tests %d: FAILURE!!\n", i, KRED);
    }
}


/*  */

//
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