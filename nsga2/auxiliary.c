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

void copy_motif_node(motif_node_t *motif1, motif_node_t *motif2){
    motif2->motif_id = motif1->motif_id;
    motif2->motif_type = motif1->motif_type;
    motif2->initial_global_index = motif1->initial_global_index;
}

void copy_motifs(individual *ind1, individual *ind2){
    int i;

    ind2->n_motifs = ind1->n_motifs;

    // allocate memory for motifs
    ind2->motifs = (motif_node_t *)malloc(ind2->n_motifs * sizeof(motif_node_t));

    for(i = 0; i<ind2->n_motifs; i++){
        copy_motif_node(&(ind1->motifs[i]), &(ind2->motifs[i]));
    }
}

void copy_neuron_node(neuron_node_t *neuron_node1, neuron_node_t *neuron_node2){
    neuron_node2->threshold = neuron_node1->threshold;
    neuron_node2->refract_time = neuron_node1->refract_time;
    neuron_node2->v_rest = neuron_node1->v_rest;
    neuron_node2->r = neuron_node1->r;
}

void copy_neuron_nodes(individual *ind1, individual *ind2){
    int i;

    neuron_node_t *neuron_node1, *neuron_node2;

    ind2->n_neurons = ind1->n_neurons;

    ind2->neurons = (neuron_node_t *)malloc(sizeof(neuron_node_t));
    neuron_node1 = ind1->neurons;
    neuron_node2 = ind2->neurons;

    copy_neuron_node(neuron_node1, neuron_node2);

    for(i = 1; i<ind2->n_neurons; i++){
        neuron_node1 = neuron_node1->next_neuron;
        neuron_node2->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t));
        neuron_node2 = neuron_node2->next_neuron;

        copy_neuron_node(neuron_node1, neuron_node2);
    }
}

void copy_synapse_node(sparse_matrix_node_t *synapse1, sparse_matrix_node_t *synapse2){
    synapse2->row = synapse1->row;
    synapse2->col = synapse1->col;
    synapse2->value = synapse1->value;

    synapse2->latency = synapse1->latency;
    synapse2->weight = synapse1->weight;
    synapse2->learning_rule = synapse1->learning_rule;
}

void copy_synapse_nodes(individual *ind1, individual *ind2){
    int i;

    sparse_matrix_node_t *synapse1, *synapse2;
    
    ind2->n_synapses = ind1->n_synapses;

    // allocate memory
    ind2->connectivity_matrix = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));

    synapse1 = ind1->connectivity_matrix;
    synapse2 = ind2->connectivity_matrix;

    // copy the first one
    copy_synapse_node(synapse1, synapse2);

    for(i = 1; i<ind2->n_synapses; i++){
        synapse1 = synapse1->next_element;

        synapse2->next_element = (sparse_matrix_node_t *)malloc(sizeof(sparse_matrix_node_t));
        synapse2 = synapse2->next_element;
        copy_synapse_node(synapse1, synapse2);
    }
}

void copy_individual(individual *ind1, individual *ind2){
    int i;

    copy_motifs(ind1, ind2);
    copy_neuron_nodes(ind1, ind2);
    copy_synapse_nodes(ind1, ind2);
}

void print_matrix_from_dynamic_list(sparse_matrix_node_t *matrix_node, int n_neurons){
    int i,j;

    int *matrix = (int *)calloc(n_neurons * n_neurons, sizeof(int));

    while(matrix_node->value != 0){
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
}

void print_individuals(NSGA2Type *nsga2Params, population *pop){
    int i, j;
    new_motif_t *motif_node;
    sparse_matrix_node_t *matrix_node;
    
    printf("\n ================================== \n");
    printf(" Printing population information... \n");

    for(i = 0; i<nsga2Params->popsize; i++){
        printf(" > Printing individual %d\n", i);
        printf("   > Number of motifs: %d\n", parent_pop->ind[i].n_motifs);
        printf("   > Printing motifs: ");

        motif_node = pop->ind[i].motifs_new;
        for(j = 0; j<pop->ind[i].n_motifs; j++){
            printf("%d ", motif_node->motif_type);
            motif_node = motif_node->next_motif;
        }
        printf("\n");

        // TODO: Print neurons too

        printf("   > Printing matrix elements (synapses):\n");
        matrix_node = pop->ind[i].connectivity_matrix; 

        while(matrix_node != NULL){
            printf("      > col = %d, row = %d, value = %d\n", matrix_node->col, matrix_node->row, matrix_node->value);
            matrix_node = matrix_node->next_element;
        }
        printf("\n");

        // print matrix
        print_matrix_from_dynamic_list(pop->ind[i].connectivity_matrix, pop->ind[i].n_neurons);

    }

    printf("\n Population printed!");
    printf("\n ================================== \n");
}

void print_networks(NSGA2Type *nsga2Params, population *pop){
    int i;
    
    printf("\n ================================== \n");
    printf(" Printing networks... \n");

    for(i = 0; i<2; i++){//for(i = 0; i<nsga2Params->popsize-2; i++){
        printf(" > Printing network %d\n", i);
        print_network_information(parent_pop->ind[i].snn);
    }

    printf("\n All networks printed!");
    printf("\n ================================== \n");
}