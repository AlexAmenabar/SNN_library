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
    pop->ind = (individual *)calloc(size, sizeof(individual));
    for (i=0; i<size; i++)
    {
        #ifdef DEBUG2
            printf(" > > Allocating memory for individual %d...\n", i);
            fflush(stdout);
        #endif
        
        allocate_memory_ind (nsga2Params, &(pop->ind[i]));

        #ifdef DEBUG2
            printf(" > > Memory allocated for individual %d...\n", i);
            fflush(stdout);
        #endif
    }
    return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    ind->obj = (double *)calloc(nsga2Params->nobj, sizeof(double));
    ind->snn = (spiking_nn_t *)calloc(1, sizeof(spiking_nn_t));
    
    return;
}

/* Function to deallocate memory to a population but without deallocating the snn structure */
void deallocate_memory_pop (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        #ifdef DEBUG2
            printf(" > > Deallocating memory for individual %d (not SNN structure)...\n", i);
            fflush(stdout);
        #endif

        deallocate_memory_ind (nsga2Params, &(pop->ind[i]));

        #ifdef DEBUG2
            printf(" > > Memory deallocated %d (not SNN structure)...\n", i);
            fflush(stdout);
        #endif
    }
    free (pop->ind);
    pop->ind = NULL;

    return;
}

/* Function to deallocate memory to an individual but without including the snn structure */
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind)
{
    int i;
    new_motif_t *old_motif_node, *motif_node;
    neuron_node_t *old_neuron_node, *neuron_node;
    sparse_matrix_node_t *old_matrix_node, *matrix_node;
    learning_zone_t *old_learning_zone, *learning_zone;
    int_node_t *int_node, *tmp_node;

    // deallocate memory of dynamic lists
    #ifdef DEBUG3
    printf(" > > > > Deallocating motifs...\n");
    fflush(stdout);
    #endif

    if(ind->motifs_new){
        motif_node = ind->motifs_new;
        while(motif_node){
            old_motif_node = motif_node;
            motif_node = motif_node->next_motif;
            free(old_motif_node);
        }
    }
    ind->motifs_new = NULL;

    /*#ifdef DEBUG3
    printf(" > > > > Deallocating learning zones...\n");
    fflush(stdout);
    #endif
    learning_zone = ind->learning_zones;
    while(learning_zone != NULL){
        old_learning_zone = learning_zone;
        learning_zone = learning_zone->next_zone;
        free(old_learning_zone);
    }*/

    #ifdef DEBUG3
    printf(" > > > > Deallocating neurons...\n");
    fflush(stdout);
    #endif

    if(ind->neurons){
        neuron_node = ind->neurons;
        while(neuron_node){
            old_neuron_node = neuron_node;
            neuron_node = neuron_node->next_neuron;
            free(old_neuron_node);
        }
    }
    ind->neurons = NULL;

    #ifdef DEBUG3
    printf(" > > > > Deallocating synapses (not input)...\n");
    fflush(stdout);
    #endif

    if(ind->connectivity_matrix){
        matrix_node = ind->connectivity_matrix;
        while(matrix_node){
            old_matrix_node = matrix_node;
            matrix_node = matrix_node->next_element;

            deallocate_sparse_matrix_node(old_matrix_node);
        }
    }
    ind->connectivity_matrix = NULL;
   
    #ifdef DEBUG3
    printf(" > > > > Deallocating input synapses...\n");
    fflush(stdout);
    #endif

    if(ind->input_synapses){
        matrix_node = ind->input_synapses;
        while(matrix_node){
            old_matrix_node = matrix_node;
            matrix_node = matrix_node->next_element;

            deallocate_sparse_matrix_node(old_matrix_node);
        }
    }
    ind->input_synapses = NULL;

    #ifdef DEBUG3
    printf(" > > > > Deallocating dynamic lists...\n");
    fflush(stdout);
    #endif

    for(i = 0; i<ind->n_motifs; i++){

        // input
        int_node = ind->connectivity_info.in_connections[i].first_node;
        while(int_node){
            tmp_node = int_node;
            int_node = int_node->next;

            free(tmp_node);
        }
        ind->connectivity_info.in_connections[i].first_node = NULL;
        ind->connectivity_info.in_connections[i].n_nodes = 0;

        // output
        int_node = ind->connectivity_info.out_connections[i].first_node;
        while(int_node){
            tmp_node = int_node;
            int_node = int_node->next;

            free(tmp_node);
        }
        ind->connectivity_info.out_connections[i].first_node = NULL;
        ind->connectivity_info.out_connections[i].n_nodes = 0;
    }
    if(ind->connectivity_info.in_connections)
        free(ind->connectivity_info.in_connections);
    if(ind->connectivity_info.out_connections)
        free(ind->connectivity_info.out_connections);

    ind->connectivity_info.in_connections = NULL;
    ind->connectivity_info.out_connections = NULL;

    #ifdef DEBUG3
    printf(" > > > > Deallocating obj functions...\n");
    fflush(stdout);
    #endif


    if(ind->obj)
        free(ind->obj);

    ind->obj = NULL;

    // deallocate SNN structure memory too 
    if(ind->snn)
        deallocate_memory_snn_only(nsga2Params, ind);


    #ifdef DEBUG3
    printf(" > > > > Individual deallocated!\n");
    #endif

    return;
}


/* Function to deallocate memory of the snn structure for a population */
void deallocate_memory_pop_snn_only (NSGA2Type *nsga2Params, population *pop, int size)
{
    int i;
    for (i=0; i<size; i++)
    {
        #ifdef DEBUG2
            printf(" > > Deallocating memory for individual %d SNN structure...\n", i);
            fflush(stdout);
        #endif

        deallocate_memory_snn_only (nsga2Params, &(pop->ind[i]));

        #ifdef DEBUG2
            printf(" > > Memory deallocated %d for SNN structure...\n", i);
            fflush(stdout);
        #endif
    }

    return;
}

/* Function to deallocate snn structure memory to an individual */
void deallocate_memory_snn_only (NSGA2Type *nsga2Params, individual *ind)
{
    free_snn_struct_memory(ind->snn);
    ind->snn = NULL;

    return;
}



/* int_array and list_int_array struct */
void allocate_int_array(int_array_t *int_array, int n){
    int_array->n = n;
    int_array->array = (int *)calloc(n, sizeof(int));
}

/* Function to deallocate an int_array that only contains one int_array */
void deallocate_int_array(int_array_t *int_array){
    free(int_array->array);
    //free(int_array);
}

/* Function to deallocate a set of int_arrays in the pointer */
void deallocate_int_arrays(int_array_t *int_arrays, int n){
    int i;

    for(i = 0; i<n; i++){
        deallocate_int_array(&(int_arrays[i]));
    }
    free(int_arrays);
    int_arrays = NULL;
}


// ACTUALLY NOT USED, BUT THESE FUNCTIONS SHOULD BE USED IN THE FUTURE 

void allocate_list_int_array(int_array_list_t *list_int_array, int n){
    list_int_array->n = n;
    list_int_array->int_array = (int_array_t *)calloc(n, sizeof(int_array_t)); // allocate memory for various int_array_t structs
}

void deallocate_list_int_array(int_array_list_t *list_int_array, int n){
    int i;
    for(i = 0; i<list_int_array->n; i++){
        deallocate_int_array(&(list_int_array->int_array[i]));
    }
    free(list_int_array);
    list_int_array = NULL;
}

void deallocate_sparse_matrix_node(sparse_matrix_node_t *sparse_matrix_node){
    free(sparse_matrix_node->latency);
    free(sparse_matrix_node->learning_rule);
    free(sparse_matrix_node->weight);
    free(sparse_matrix_node);
}
