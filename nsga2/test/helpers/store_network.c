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

    FILE *network_file = fopen("./test_network.txt", "w");
    store_individual_in_file(network_file, ind);
}


int main(int argc, char **argv){
    int results[4];
    
    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: store individual\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    
    print_result(results[0], 0);
    
    printf("\n");
}