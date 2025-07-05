/* Memory allocation and deallocation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"
# include "snn_library.h"

/* Function to allocate memory to a population */
void allocate_memory_pop (NSGA2Type *nsga2Params,  population *pop, int size)
{
    int i;
    pop->ind = (individual *)malloc(size*sizeof(individual));
    for (i=0; i<size; i++)
    {
        allocate_memory_ind (nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    ind->obj = (double *)malloc(nsga2Params->nobj*sizeof(double));
    ind->snn = (spiking_nn_t *)malloc(sizeof(spiking_nn_t));

    return;
}

/* Function to deallocate memory to a population but without including the snn structure */
void deallocate_memory_pop (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        deallocate_memory_ind (nsga2Params, &(pop->ind[i]));
    }
    free (pop->ind);

    return;
}

/* Function to deallocate memory to an individual but without including the snn structure */
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind)
{
    new_motif_t *old_motif_node, *motif_node;
    neuron_node_t *old_neuron_node, *neuron_node;
    sparse_matrix_node_t *old_matrix_node, *matrix_node;
    learning_zone_t *old_learning_zone, *learning_zone;
    
    free(ind->obj);

    // deallocate memory of dynamic lists
    motif_node = ind->motifs_new;
    while(motif_node != NULL){
        old_motif_node = motif_node;
        motif_node = motif_node->next_motif;
        free(old_motif_node);
    }

    learning_zone = ind->learning_zones;
    while(learning_zone != NULL){
        old_learning_zone = learning_zone;
        learning_zone = learning_zone->next_zone;
        free(old_learning_zone);
    }

    neuron_node = ind->neurons;
    while(neuron_node != NULL){
        old_neuron_node = neuron_node;
        neuron_node = neuron_node->next_neuron;
        free(old_neuron_node);
    }

    matrix_node = ind->connectivity_matrix;
    while(matrix_node != NULL){
        old_matrix_node = matrix_node;
        matrix_node = matrix_node->next_element;
        free(old_matrix_node);
    }
   
    matrix_node = ind->input_synapses;
    while(matrix_node != NULL){
        old_matrix_node = matrix_node;
        matrix_node = matrix_node->next_element;
        free(old_matrix_node);
    }

    return;
}



/* Function to deallocate memory to a population */
void deallocate_memory_pop_snn_included (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        deallocate_memory_ind_snn_included (nsga2Params, &(pop->ind[i]));
    }
    free (pop->ind);

    return;
}

/* Function to deallocate memory to an individual */
void deallocate_memory_ind_snn_included (NSGA2Type *nsga2Params, individual *ind)
{
    new_motif_t *old_motif_node, *motif_node;
    neuron_node_t *old_neuron_node, *neuron_node;
    sparse_matrix_node_t *old_matrix_node, *matrix_node;
    learning_zone_t *old_learning_zone, *learning_zone;
    
    free(ind->obj);

    free_snn_struct_memory(ind->snn);
    
    // deallocate memory of dynamic lists
    motif_node = ind->motifs_new;
    while(motif_node != NULL){
        old_motif_node = motif_node;
        motif_node = motif_node->next_motif;
        free(old_motif_node);
    }

    learning_zone = ind->learning_zones;
    while(learning_zone != NULL){
        old_learning_zone = learning_zone;
        learning_zone = learning_zone->next_zone;
        free(old_learning_zone);
    }

    neuron_node = ind->neurons;
    while(neuron_node != NULL){
        old_neuron_node = neuron_node;
        neuron_node = neuron_node->next_neuron;
        free(old_neuron_node);
    }

    matrix_node = ind->connectivity_matrix;
    while(matrix_node != NULL){
        old_matrix_node = matrix_node;
        matrix_node = matrix_node->next_element;
        free(old_matrix_node);
    }
   
    matrix_node = ind->input_synapses;
    while(matrix_node != NULL){
        old_matrix_node = matrix_node;
        matrix_node = matrix_node->next_element;
        free(old_matrix_node);
    }

    return;
}


/* Function to deallocate memory of the snn structure for a population */
void deallocate_memory_pop_snn_only (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        deallocate_memory_snn_only (nsga2Params, &(pop->ind[i]));
    }

    return;
}

/* Function to deallocate snn structure memory to an individual */
void deallocate_memory_snn_only (NSGA2Type *nsga2Params, individual *ind)
{
    free_snn_struct_memory(ind->snn);
    return;
}


void deallocate_int_array(int_array_t *int_array){
    free(int_array->array);
    free(int_array);
    int_array = NULL;
}