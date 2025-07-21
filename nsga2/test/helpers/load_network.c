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

    // sparse matrix
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

    target_matrix[57] = 1;
    target_matrix[61] = 1;
    target_matrix[63] = 1;
    target_matrix[64] = 1;
    target_matrix[65] = 1;
    target_matrix[73] = -1;
    target_matrix[79] = 1;
    target_matrix[85] = 1;
    target_matrix[86] = 1;
    target_matrix[87] = 1;

    target_matrix[88] = 1;
    target_matrix[93] = 1;
    target_matrix[95] = 1;
    target_matrix[96] = 2;
    target_matrix[97] = 3;
    target_matrix[98] = 3;
    target_matrix[99] = 1;
    target_matrix[104] = 1;
    target_matrix[106] = 1;
    target_matrix[107] = 3;
    target_matrix[108] = 2;
    target_matrix[109] = 3;
    target_matrix[110] = 1;
    target_matrix[115] = 1;
    target_matrix[117] = 1;
    target_matrix[118] = 3;
    target_matrix[119] = 3;
    target_matrix[120] = 2;


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
            || motif_node->next_motif->next_motif->next_motif->next_motif != NULL){
            equals = 0;
        }

        if(ind->n_motifs != 4 || ind->n_neurons != 11 || ind->n_synapses - ind->n_input_synapses != 53)
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
        if(ind->connectivity_info.in_connections[0].n_nodes != 3 || ind->connectivity_info.in_connections[1].n_nodes != 1 ||
            ind->connectivity_info.in_connections[2].n_nodes != 3 || ind->connectivity_info.in_connections[3].n_nodes != 3)
            equals = 0;

        if(ind->connectivity_info.out_connections[0].n_nodes != 1 || ind->connectivity_info.out_connections[1].n_nodes != 3 ||
            ind->connectivity_info.out_connections[2].n_nodes != 2 || ind->connectivity_info.out_connections[3].n_nodes != 4)
            equals = 0;

        if(ind->connectivity_info.in_connections[0].first_node->value != 1 || ind->connectivity_info.in_connections[0].first_node->next->value != 1 ||
           ind->connectivity_info.in_connections[0].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[0].first_node->next->next->next != NULL ||
           ind->connectivity_info.in_connections[1].first_node->value != 2 || ind->connectivity_info.in_connections[1].first_node->next != NULL ||
           ind->connectivity_info.in_connections[2].first_node->value != 0 || ind->connectivity_info.in_connections[2].first_node->next->value != 1 ||
           ind->connectivity_info.in_connections[2].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[2].first_node->next->next->next != NULL ||
           ind->connectivity_info.in_connections[3].first_node->value != 2 || ind->connectivity_info.in_connections[3].first_node->next->value != 3 ||
           ind->connectivity_info.in_connections[3].first_node->next->next->value != 3 || ind->connectivity_info.in_connections[3].first_node->next->next->next != NULL)
           equals = 0;

        if(ind->connectivity_info.out_connections[0].first_node->value != 2 || ind->connectivity_info.out_connections[0].first_node->next != NULL || 
           ind->connectivity_info.out_connections[1].first_node->value != 0 || ind->connectivity_info.out_connections[1].first_node->next->value != 0 || 
           ind->connectivity_info.out_connections[1].first_node->next->next->value != 2 || ind->connectivity_info.out_connections[1].first_node->next->next->next != NULL || 
           ind->connectivity_info.out_connections[2].first_node->value != 1 || ind->connectivity_info.out_connections[2].first_node->next->value != 3 || 
           ind->connectivity_info.out_connections[2].first_node->next->next != NULL ||
           ind->connectivity_info.out_connections[3].first_node->value != 0 || ind->connectivity_info.out_connections[3].first_node->next->value != 2 ||
           ind->connectivity_info.out_connections[3].first_node->next->next->value != 3 || ind->connectivity_info.out_connections[3].first_node->next->next->next->value != 3 ||
           ind->connectivity_info.out_connections[3].first_node->next->next->next->next != NULL)
           equals = 0;
    }


    return equals;
}


int main(int argc, char **argv){
    int results[4];
    
    initialize_motifs();

    printf("   = == ====== == ====== == =   \nStarting tests: build sparse matrix\n   = == ====== == ====== == =   \n");

    results[0] = test1();
    
    print_result(results[0], 0);
    
    printf("\n");
}