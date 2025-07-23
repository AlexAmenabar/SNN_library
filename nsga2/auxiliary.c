/* Some utility functions (not part of the algorithm) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

/* Function to return the maximum of two variables */
double maximum (double a, double b)
{
    if (a>b)
    {
        return(a);
    }
    return (b);
}

/* Function to return the minimum of two variables */
double minimum (double a, double b)
{
    if (a<b)
    {
        return (a);
    }
    return (b);
}

void print_double_matrix(double *matrix, int n){
    int i, j;
    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            printf("%f ", matrix[i * n + j]);
        }
        printf("\n");
    }
}


/* Functions to copy individuals */

void copy_individual(individual *ind1, individual *ind2){
    
    #ifdef DEBUG2
    printf(" > > Copying individual...\n");
    fflush(stdout);
    #endif

    ind2->n_motifs = ind1->n_motifs;
    ind2->n_neurons = ind1->n_neurons;
    ind2->n_input_neurons = ind1->n_input_neurons;
    ind2->n_synapses = ind1->n_synapses;
    ind2->n_input_synapses = ind1->n_input_synapses;

    #ifdef DEBUG3
    printf(" > > > Copying motifs...\n");
    fflush(stdout);
    #endif 

    copy_motifs(ind1, ind2);

    #ifdef DEBUG3
    printf(" > > > Copying neuron nodes...\n");
    fflush(stdout);
    #endif 

    copy_neuron_nodes(ind1, ind2);

    // connect motifs and nodes

    #ifdef DEBUG3
    printf(" > > > Connecting motifs and neurons...\n");
    fflush(stdout);
    #endif 

    connect_motifs_and_neurons(ind2);


    #ifdef DEBUG3
    printf(" > > > Copying synapses...\n");
    fflush(stdout);
    #endif 
    copy_synapse_nodes(ind1, ind2);


    #ifdef DEBUG3
    printf(" > > > Copying connectivity (dynamic lists)...\n");
    fflush(stdout);
    #endif 
    copy_connectivity(ind1, ind2); // copy the list of what motifs are connected

    #ifdef DEBUG2
    printf(" > > Individual copied!\n");
    fflush(stdout);
    #endif

    return;
}

void copy_motifs(individual *ind1, individual *ind2){
    new_motif_t *original_motif, *copied_motif;
    
    // copy the number of motifs
    ind2->n_motifs = ind1->n_motifs;

    //printf(" >>>>>> N motifs: %d\n", ind1->n_motifs);
    fflush( stdout );

    // create motifs dynamic list
    original_motif = ind1->motifs_new;
    
    //printf(" >>>>> Allocating memory for motifs\n");
    ind2->motifs_new = (new_motif_t *)malloc(sizeof(new_motif_t));
    copied_motif = ind2->motifs_new;
    //printf(" >>>>> Memory allocated\n");
    //ind2->n_input_motifs_per_motif[0] = ind1->n_input_motifs_per_motif[0];
    //ind2->n_output_motifs_per_motif[0] = ind1->n_output_motifs_per_motif[0];

    // copy the first element
    copy_motif_node(original_motif, copied_motif);
    //printf(" >>>>> First motif copied\n");

    // copy the rest of motifs
    //printf(" >>>>> Copying motifs...\n");
    for(int i = 1; i<ind1->n_motifs; i++){
        original_motif = original_motif->next_motif;
        
        copied_motif->next_motif = (new_motif_t *)malloc(sizeof(new_motif_t));
        copied_motif = copied_motif->next_motif;

        copy_motif_node(original_motif, copied_motif);
    }
}

void copy_motif_node(new_motif_t *motif1, new_motif_t *motif2){
    motif2->motif_id = motif1->motif_id;
    motif2->motif_type = motif1->motif_type;
    motif2->initial_global_index = motif1->initial_global_index;

    motif2->next_motif = NULL;
}

void copy_neuron_nodes(individual *ind1, individual *ind2){
    neuron_node_t *original_neuron_node, *copied_neuron_node;

    // copu the number of neurons
    ind2->n_neurons = ind1->n_neurons;
    ind2->n_input_neurons = ind1->n_input_neurons;

    // 
    original_neuron_node = ind1->neurons;

    ind2->neurons = (neuron_node_t *)malloc(sizeof(neuron_node_t));
    copied_neuron_node = ind2->neurons;

    // copy the first neuron
    copy_neuron_node(original_neuron_node, copied_neuron_node);

    // copy the rest neuron
    for(int i = 1; i<ind2->n_neurons; i++){
        original_neuron_node = original_neuron_node->next_neuron;

        copied_neuron_node->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t));
        copied_neuron_node = copied_neuron_node->next_neuron;

        copy_neuron_node(original_neuron_node, copied_neuron_node);
    }
}

void copy_neuron_node(neuron_node_t *neuron_node1, neuron_node_t *neuron_node2){
    neuron_node2->threshold = neuron_node1->threshold;
    neuron_node2->refract_time = neuron_node1->refract_time;
    neuron_node2->v_rest = neuron_node1->v_rest;
    neuron_node2->r = neuron_node1->r;
    neuron_node2->behaviour = neuron_node1->behaviour;

    neuron_node2->next_neuron = NULL;
}

void copy_synapse_nodes(individual *ind1, individual *ind2){
    sparse_matrix_node_t *original_synapse, *copied_synapse;
    
    ind2->n_synapses = ind1->n_synapses;
    ind2->n_input_synapses = ind1->n_input_synapses;


    // allocate memory
    ind2->connectivity_matrix = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    original_synapse = ind1->connectivity_matrix;
    copied_synapse = ind2->connectivity_matrix;

    // copy the first one
    copy_synapse_node(original_synapse, copied_synapse);

    while(original_synapse->next_element != NULL){
        original_synapse = original_synapse->next_element;

        copied_synapse->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
        copied_synapse = copied_synapse->next_element;
        copy_synapse_node(original_synapse, copied_synapse);
    }

    // Now the same for input synapses
    //printf(" >>>>>> In input synapses...\n");

    ind2->input_synapses = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    original_synapse = ind1->input_synapses;
    copied_synapse = ind2->input_synapses;

    // copy the first one
    copy_synapse_node(original_synapse, copied_synapse);

    while(original_synapse->next_element){
        original_synapse = original_synapse->next_element;

        copied_synapse->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
        copied_synapse = copied_synapse->next_element;
        copy_synapse_node(original_synapse, copied_synapse);
    }
}

void copy_synapse_node(sparse_matrix_node_t *synapse1, sparse_matrix_node_t *synapse2){
    synapse2->row = synapse1->row;
    synapse2->col = synapse1->col;
    synapse2->value = synapse1->value;

    synapse2->latency = (uint8_t *)calloc(abs(synapse1->value), sizeof(uint8_t));
    synapse2->weight = (double *)calloc(abs(synapse1->value), sizeof(double));
    synapse2->learning_rule = (uint8_t *)calloc(abs(synapse1->value), sizeof(uint8_t));

    for(int i = 0; i<abs(synapse1->value); i++){
        synapse2->latency[i] = synapse1->latency[i];
        synapse2->weight[i] = synapse1->weight[i];
        synapse2->learning_rule[i] = synapse1->learning_rule[i];
    }

    synapse2->next_element = NULL;
}


void copy_connectivity(individual *ind1, individual *ind2){
    int i, j;
    int_node_t *int_node, *int_node_to_copy, *prev, *next;

    // allocate memory for lists and copy general information
    ind2->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind1->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind2->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind1->n_motifs * 10, sizeof(int_dynamic_list_t));
    ind2->connectivity_info.n_max_motifs = ind1->connectivity_info.n_max_motifs;


    // copy lists
    for(i = 0; i<ind2->n_motifs; i++){
        // copy number of input and output nodes for each motif
        ind2->connectivity_info.in_connections[i].n_nodes = ind1->connectivity_info.in_connections[i].n_nodes;
        ind2->connectivity_info.out_connections[i].n_nodes = ind1->connectivity_info.out_connections[i].n_nodes;


        // if number of input connections is bigger than 0, then copy the dynamic list
        if(ind2->connectivity_info.in_connections[i].n_nodes > 0){
            int_node_to_copy = ind1->connectivity_info.in_connections[i].first_node;

            // initialize first element            
            int_node = initialize_and_allocate_int_node(int_node_to_copy->value, NULL, NULL);
            ind2->connectivity_info.in_connections[i].first_node = int_node;

            int_node_to_copy = int_node_to_copy->next;

            // initialize the rest of nodes
            while(int_node_to_copy){
                int_node = initialize_and_allocate_int_node(int_node_to_copy->value, int_node, NULL); // int node is the previous for tbe new one
                int_node_to_copy = int_node_to_copy->next;
            }
        }
        
        
        // the same for the output motifs
        // if number of input connections is bigger than 0, then copy the dynamic list
        if(ind2->connectivity_info.out_connections[i].n_nodes > 0){
            int_node_to_copy = ind1->connectivity_info.out_connections[i].first_node;

            // initialize first element            
            int_node = initialize_and_allocate_int_node(int_node_to_copy->value, NULL, NULL);
            ind2->connectivity_info.out_connections[i].first_node = int_node;

            int_node_to_copy = int_node_to_copy->next;

            // initialize the rest of nodes

            while(int_node_to_copy){
                int_node = initialize_and_allocate_int_node(int_node_to_copy->value, int_node, NULL); // int node is the previous for tbe new one
                int_node_to_copy = int_node_to_copy->next;
            }
        }
    }
}


/**
 * Storage
 */

void load_individual_from_file(FILE *f, individual *ind){
    
    int i, j;

    // load general information
    fscanf(f, "%d", &(ind->n_motifs));
    fscanf(f, "%d", &(ind->n_neurons));
    fscanf(f, "%d", &(ind->n_input_neurons));
    fscanf(f, "%d", &(ind->n_synapses));
    fscanf(f, "%d", &(ind->n_input_synapses));
    printf("General information loaded\n");
    fflush(stdout);

    // load motifs
    ind->motifs_new = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    new_motif_t *motif_node = ind->motifs_new;

    fscanf(f, "%d", &(motif_node->motif_type));
    fscanf(f, "%d", &(motif_node->initial_global_index));
    motif_node->next_motif = NULL;

    for(i = 1; i<ind->n_motifs; i++){
        motif_node->next_motif = (new_motif_t *)calloc(1, sizeof(new_motif_t));
        motif_node = motif_node->next_motif;

        fscanf(f, "%d", &(motif_node->motif_type));
        fscanf(f, "%d", &(motif_node->initial_global_index));

        motif_node->next_motif = NULL;
    }
    printf("Motifs loaded\n");
    fflush(stdout);

    // load neurons
    ind->neurons = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
    neuron_node_t *neuron_node = ind->neurons;

    fscanf(f, "%lf", &(neuron_node->threshold));
    fscanf(f, "%lf", &(neuron_node->v_rest));
    fscanf(f, "%d", &(neuron_node->refract_time));
    fscanf(f, "%lf", &(neuron_node->r));
    fscanf(f, "%d", &(neuron_node->behaviour));
    neuron_node->next_neuron = NULL;

    for(i = 1; i<ind->n_neurons; i++){
        neuron_node->next_neuron = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
        neuron_node = neuron_node->next_neuron;

        fscanf(f, "%lf", &(neuron_node->threshold));
        fscanf(f, "%lf", &(neuron_node->v_rest));
        fscanf(f, "%d", &(neuron_node->refract_time));
        fscanf(f, "%lf", &(neuron_node->r));
        fscanf(f, "%d", &(neuron_node->behaviour));
        
        neuron_node->next_neuron = NULL;
    }
    printf("Neurons loaded\n");
    fflush(stdout);
    // connect the motifs and the neurons
    connect_motifs_and_neurons(ind);
    printf("Neurons and motifs connected\n");
    fflush(stdout);

    // load dynamic lists of connectivity
    int_node_t *int_node, *prev_int_node;

    ind->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));
    ind->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));

    for(i = 0; i<ind->n_motifs; i++){
        printf(" In motif %d\n", i);
        fscanf(f, "%d", &(ind->connectivity_info.in_connections[i].n_nodes));
        fscanf(f, "%d", &(ind->connectivity_info.out_connections[i].n_nodes));
    
        // load input nodes
        ind->connectivity_info.in_connections[i].first_node = (int_node_t *)calloc(1, sizeof(int_node_t));
        int_node = ind->connectivity_info.in_connections[i].first_node;

        fscanf(f, "%d", &(int_node->value));
        int_node->prev = NULL;
        int_node->next = NULL;
        
        for(j = 1; j<ind->connectivity_info.in_connections[i].n_nodes; j++){
            prev_int_node = int_node;
            int_node->next = (int_node_t *)calloc(1, sizeof(int_node_t));
            int_node = int_node->next;
            int_node->next = NULL;
            int_node->prev = prev_int_node;

            fscanf(f, "%d", &(int_node->value));
        }

        // load output nodes
        ind->connectivity_info.out_connections[i].first_node = (int_node_t *)calloc(1, sizeof(int_node_t));
        int_node = ind->connectivity_info.out_connections[i].first_node;

        fscanf(f, "%d", &(int_node->value));
        int_node->prev = NULL;
        int_node->next = NULL;
        
        for(j = 1; j<ind->connectivity_info.out_connections[i].n_nodes; j++){
            prev_int_node = int_node;
            int_node->next = (int_node_t *)calloc(1, sizeof(int_node_t));
            int_node = int_node->next;
            int_node->next = NULL;
            int_node->prev = prev_int_node;

            fscanf(f, "%d", &(int_node->value));
        }
    }
    printf("Connectivity loaded\n");
    fflush(stdout);
    // load synapses
    ind->connectivity_matrix = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));

    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
    synapse_node->learning_rule = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));

    i = 0;
    while(i < abs(synapse_node->value)){
        fscanf(f, "%lf", &(synapse_node->weight[i]));
        fscanf(f, "%d", &(synapse_node->latency[i]));
        fscanf(f, "%d", &(synapse_node->learning_rule[i]));
        i++;
    }
    synapse_node->next_element = NULL;

    while(i<ind->n_synapses-ind->n_input_synapses){
        synapse_node->next_element = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
        synapse_node = synapse_node->next_element;
        fscanf(f, "%d", &(synapse_node->row));
        fscanf(f, "%d", &(synapse_node->col));
        fscanf(f, "%d", &(synapse_node->value));
        
        synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
        synapse_node->latency = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
        synapse_node->learning_rule = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
        for(j = 0; j<abs(synapse_node->value); j++){
            fscanf(f, "%lf", &(synapse_node->weight[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));
            i++;
        }
        synapse_node->next_element = NULL;
    }
    printf("Synapses loaded\n");
    fflush(stdout);
    // load input synapses
    ind->input_synapses = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    synapse_node = ind->input_synapses;

    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));
    
    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
    synapse_node->learning_rule = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));

    i = 0;
    while(i < abs(synapse_node->value)){
        fscanf(f, "%lf", &(synapse_node->weight[i]));
        fscanf(f, "%d", &(synapse_node->latency[i]));
        fscanf(f, "%d", &(synapse_node->learning_rule[i]));
        i++;
    }
    synapse_node->next_element = NULL;

    while(i<ind->n_input_synapses){
        synapse_node->next_element = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
        synapse_node = synapse_node->next_element;
        fscanf(f, "%d", &(synapse_node->row));
        fscanf(f, "%d", &(synapse_node->col));
        fscanf(f, "%d", &(synapse_node->value));
        
        synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
        synapse_node->latency = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
        synapse_node->learning_rule = (uint8_t *)malloc(abs(synapse_node->value) * sizeof(uint8_t));
        for(j = 0; j<abs(synapse_node->value); j++){
            fscanf(f, "%lf", &(synapse_node->weight[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));
            i++;
        }
        synapse_node->next_element = NULL;
    }
    printf("Input synapses loaded\n");
    fflush(stdout);
}


void store_individual_in_file(FILE *f, individual *ind){
    
    int i, j;

    // store general information
    fprintf(f, "%d\n", ind->n_motifs);
    fprintf(f, "%d\n", ind->n_neurons);
    fprintf(f, "%d\n", ind->n_input_neurons);
    fprintf(f, "%d\n", ind->n_synapses);
    fprintf(f, "%d\n", ind->n_input_synapses);
    fprintf(f, "\n");

    printf("General information stored\n");

    // store motifs
    new_motif_t *motif_node = ind->motifs_new;
    while(motif_node){
        fprintf(f, "%d ", motif_node->motif_type);
        fprintf(f, "%d ", motif_node->initial_global_index);
        fprintf(f, "\n");
        motif_node = motif_node->next_motif;
    }
    fprintf(f, "\n");
    printf("Motifs stored\n");

    // store neurons
    neuron_node_t *neuron_node = ind->neurons;
    while(neuron_node){
        fprintf(f, "%lf ", neuron_node->threshold);
        fprintf(f, "%lf ", neuron_node->v_rest);
        fprintf(f, "%d ", neuron_node->refract_time);
        fprintf(f, "%lf ", neuron_node->r);
        fprintf(f, "%d ", neuron_node->behaviour);
        fprintf(f , "\n");
        neuron_node = neuron_node->next_neuron;
    }
    fprintf(f, "\n");
    printf("Neurons stored\n");

    // store dynamic lists of connectivity
    int_node_t *int_node;
    for(i = 0; i<ind->n_motifs; i++){
        fprintf(f, "%d ", ind->connectivity_info.in_connections[i].n_nodes);
        fprintf(f, "%d\n", ind->connectivity_info.out_connections[i].n_nodes);
    
        // store input nodes
        int_node = ind->connectivity_info.in_connections[i].first_node;

        while(int_node){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n");

        int_node = ind->connectivity_info.out_connections[i].first_node;

        while(int_node){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n\n");
    }
    printf("Connectivity stored\n");

    // store synapses
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    while(synapse_node){
        fprintf(f, "%d ", synapse_node->row);
        fprintf(f, "%d ", synapse_node->col);
        fprintf(f, "%d ", synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            fprintf(f, "%lf ", synapse_node->weight[i]);
            fprintf(f, "%d ", synapse_node->latency[i]);
            fprintf(f, "%d ", synapse_node->learning_rule[i]);
        }
        fprintf(f, "\n");
        synapse_node = synapse_node->next_element;
    }
    fprintf(f, "\n");
    printf("Synapses stored\n");

    // store input synapses
    synapse_node = ind->input_synapses;

    while(synapse_node){
        fprintf(f, "%d ", synapse_node->row);
        fprintf(f, "%d ", synapse_node->col);
        fprintf(f, "%d ", synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            fprintf(f, "%lf ", synapse_node->weight[i]);
            fprintf(f, "%d ", synapse_node->latency[i]);
            fprintf(f, "%d ", synapse_node->learning_rule[i]);
        }
        fprintf(f, "\n");
        synapse_node = synapse_node->next_element;
    }
    fprintf(f, "\n");
    printf("Input synapses stored\n");
}


/**
 * Helpers
 */
void get_complete_matrix_from_dynamic_list(int *complete_matrix, sparse_matrix_node_t *matrix_node, int n_neurons){
    int i,j;

    while(matrix_node != NULL){
        //printf(" matrix_node->row = %d, col = %d\n", matrix_node->row, matrix_node->col);
        //fflush(stdout);
        complete_matrix[matrix_node->row * n_neurons + matrix_node->col] = matrix_node->value;
        matrix_node = matrix_node->next_element;
    }
}

void print_matrix_from_dynamic_list(sparse_matrix_node_t *matrix_node, int n_neurons){
    int i,j;

    int *matrix = (int *)calloc(n_neurons * n_neurons, sizeof(int));

    while(matrix_node != NULL){
        matrix[matrix_node->row * n_neurons + matrix_node->col] = matrix_node->value;
        matrix_node = matrix_node->next_element;
    }

    for(i = 0; i<n_neurons; i++){
        for(j = 0; j<n_neurons; j++){
            printf("%d ", matrix[i * n_neurons + j]);
        }
        printf("\n");
    }
    printf("\n\n");

    // free memory
    free(matrix);
}

void print_connectivity_matrix(int *matrix, int n){
    int i,j;

    for(i = 0; i<n; i++){
        for(j = 0; j<n; j++){
            printf("%d ", matrix[i * n + j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void print_synapses_dynamic_list(sparse_matrix_node_t *matrix_node){
    while(matrix_node != NULL){
        printf(" >>>> col = %d, row = %d, value = %d\n", matrix_node->col, matrix_node->row, matrix_node->value);
        fflush(stdout);
        matrix_node = matrix_node->next_element;
    }
    printf("\n");
}

void print_individuals(NSGA2Type *nsga2Params, population *pop){
    int i, j;
    new_motif_t *motif_node;
    sparse_matrix_node_t *matrix_node;
    
    printf("================================== \n");
    printf(" > Printing population information... \n");

    for(i = 0; i<nsga2Params->popsize; i++){
        printf(" >> Printing individual %d\n", i);
        printf(" >>> Number of motifs: %d\n", parent_pop->ind[i].n_motifs);
        printf(" >>> Printing motifs: ");

        motif_node = pop->ind[i].motifs_new;
        for(j = 0; j<pop->ind[i].n_motifs; j++){
            printf("%d ", motif_node->motif_type);
            motif_node = motif_node->next_motif;
        }
        printf("\n");

        // TODO: Print neurons too

        printf(" >>> Printing matrix elements (synapses):\n");
        matrix_node = pop->ind[i].connectivity_matrix; 

        while(matrix_node != NULL){
            printf(" >>>> col = %d, row = %d, value = %d\n", matrix_node->col, matrix_node->row, matrix_node->value);
            fflush(stdout);
            matrix_node = matrix_node->next_element;
        }
        printf("\n");

        // print matrix
        //print_matrix_from_dynamic_list(pop->ind[i].connectivity_matrix, pop->ind[i].n_neurons);

    }

    printf(" > Population printed!");
    printf("================================== \n");
}

void print_networks(NSGA2Type *nsga2Params, population *pop){
    int i;
    
    printf("================================== \n");
    printf(" > Printing networks... \n");

    for(i = 0; i<nsga2Params->popsize; i++){//for(i = 0; i<nsga2Params->popsize-2; i++){
        printf(" >> Printing network %d\n", i);
        print_network_information(parent_pop->ind[i].snn);
    }

    printf(" > All networks printed!\n");
    printf("================================== \n");
}