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
    printf(" >>>> Copy motifs\n");
    copy_motifs(ind1, ind2);

    printf(" >>>> Copy neuron nodes\n");
    copy_neuron_nodes(ind1, ind2);

    // connect motifs and nodes
    connect_motifs_and_neurons(ind2);

    printf(" >>>> Copy synapse nodes, input and middle ones\n");
    copy_synapse_nodes(ind1, ind2);

    printf(" >>>> Finished copying!\n");

    return;
}

void copy_motifs(individual *ind1, individual *ind2){
    new_motif_t *original_motif, *copied_motif;
    
    // copy the number of motifs
    ind2->n_motifs = ind1->n_motifs;

    printf(" >>>>>> N motifs: %d\n", ind1->n_motifs);
    fflush( stdout );

    // create motifs dynamic list
    original_motif = ind1->motifs_new;
    
    printf(" >>>>> Allocating memory for motifs\n");
    ind2->motifs_new = (new_motif_t *)malloc(sizeof(new_motif_t));
    copied_motif = ind2->motifs_new;
    printf(" >>>>> Memory allocated\n");

    // copy the first element
    copy_motif_node(original_motif, copied_motif);
    printf(" >>>>> First motif copied\n");

    // copy the rest of motifs
    printf(" >>>>> Copying motifs...\n");
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
    printf(" >>>>>> In input synapses...\n");

    ind2->input_synapses = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    original_synapse = ind1->input_synapses;
    copied_synapse = ind2->input_synapses;

    // copy the first one
    copy_synapse_node(original_synapse, copied_synapse);

    while(original_synapse->next_element != NULL){
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

void get_complete_matrix_from_dynamic_list(int *complete_matrix, sparse_matrix_node_t *matrix_node, int n_neurons){
    int i,j;

    while(matrix_node != NULL){
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