#include "../test.h"


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
     */
    
    // define neccessary variables
    individual *ind;
    
    // allocate memory for some variables
    ind = (individual *)calloc(1, sizeof(individual));
    
    FILE *network_file = fopen("./test_network.txt", "r");
    load_individual_from_file(network_file, ind);
    printf("Loaded\n");
    fflush(stdout);

    // store individual and compare files
    FILE *result_network_file = fopen("./test_load_network.txt", "w");
    store_individual_in_file(result_network_file, ind);

    return 1;
}


int main(int argc, char **argv){
    int results[4];
    
    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: load individual\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    
    print_result(results[0], 0);
    
    printf("\n");
}