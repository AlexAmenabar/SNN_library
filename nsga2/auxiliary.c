/* Some utility functions (not part of the algorithm) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
#include <stdint.h>
#include <inttypes.h>

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


#ifdef PHASE2
    
    #ifdef DEBUG2
        printf(" > > Copying learning zones...\n");
        fflush(stdout);
    #endif
    
    copy_learning_zones(ind1, ind2);

    copy_lz_for_motifs(ind1, ind2);

    connect_motifs_and_lz(ind2);
#endif    

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
    motif2->lz_index = motif1->lz_index;

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

    synapse2->latency = (int *)calloc(abs(synapse1->value), sizeof(int));
    synapse2->weight = (double *)calloc(abs(synapse1->value), sizeof(double));
    synapse2->learning_rule = (int *)calloc(abs(synapse1->value), sizeof(int));

    for(int i = 0; i<abs(synapse1->value); i++){
        synapse2->latency[i] = synapse1->latency[i];
        synapse2->weight[i] = synapse1->weight[i];
        synapse2->learning_rule[i] = synapse1->learning_rule[0];
    }
    
    synapse2->next_element = NULL;
}


void copy_connectivity(individual *ind1, individual *ind2){
    int i;
    int_node_t *int_node, *int_node_to_copy;

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

void copy_learning_zones(individual *ind1, individual *ind2){

    int i;
    learning_zone_t *or_lz, *cp_lz, *pr_lz = NULL;

    ind2->n_learning_zones = ind1->n_learning_zones;

    if(ind2->n_learning_zones > 0){
        
        ind2->learning_zones = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));
        cp_lz = ind2->learning_zones;
        or_lz = ind1->learning_zones;

        for(i = 0; i<ind2->n_learning_zones; i++){
            
            copy_learning_zone(or_lz, cp_lz);
            
            cp_lz->previous_zone = pr_lz;
            cp_lz->next_zone = NULL;

            if(pr_lz)
                pr_lz->next_zone = cp_lz;

            if(or_lz->next_zone)
                cp_lz->next_zone = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));

            pr_lz = cp_lz;
            cp_lz = cp_lz->next_zone;
            or_lz = or_lz->next_zone;
        }
    }
}

void copy_learning_zone(learning_zone_t *lz1, learning_zone_t *lz2){

    int i;

    lz2->n_lr = lz1->n_lr;

    lz2->lr = (int *)calloc(lz2->n_lr, sizeof(int));
    for(i = 0; i<lz2->n_lr; i++)
        lz2->lr[i] = lz1->lr[i];
    
    lz2->n_motifs = lz1->n_motifs;
}

void copy_lz_for_motifs(individual *ind1, individual *ind2){

    new_motif_t *m1, *m2;
    m1 = ind1->motifs_new;
    m2 = ind2->motifs_new;

    while(m1){
        m2->lz_index = m1->lz_index;
        m1 = m1->next_motif,
        m2 = m2->next_motif;
    }

}

void connect_motifs_and_lz(individual *ind){

    int i;
    new_motif_t *m;
    learning_zone_t *lz, **lz_array;

    lz_array = (learning_zone_t **)calloc(ind->n_learning_zones, sizeof(learning_zone_t *));

    lz = ind->learning_zones;
    for(i = 0; i<ind->n_learning_zones; i++){
        lz_array[i] = lz;
        lz = lz->next_zone;
    }

    m = ind->motifs_new;
    while(m){

        m->lz = lz_array[m->lz_index];

        m = m->next_motif;
    }

    free(lz_array);
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
    ind->n_learning_zones = 0;

 
#ifdef DEBUG3
    // print general information
    printf(" > > General information: n_motifs %d, n_neurons %d, n_input_neurons %d, n_synapses %d, n_input_synapses %d, n_lz %d\n",
            ind->n_motifs, ind->n_neurons, ind->n_input_neurons, ind->n_synapses, ind->n_input_synapses, ind->n_learning_zones);
    fflush(stdout);
#endif


    // load motifs
    ind->motifs_new = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    new_motif_t *motif_node = ind->motifs_new;

    //fscanf(f, "%"SCNu8, &(motif_node->motif_type));
    //fscanf(f, "%"SCNu32, &(motif_node->initial_global_index));
    fscanf(f, "%d", &(motif_node->motif_type));
    fscanf(f, "%d", &(motif_node->initial_global_index));
    
    motif_node->in_lz = 0;
    motif_node->lz_index = -1;
    motif_node->lz = NULL;

    motif_node->next_motif = NULL;

    for(i = 1; i<ind->n_motifs; i++){
        motif_node->next_motif = (new_motif_t *)calloc(1, sizeof(new_motif_t));
        motif_node = motif_node->next_motif;

        //fscanf(f, "%"SCNu8, &(motif_node->motif_type));
        //fscanf(f, "%"SCNu32, &(motif_node->initial_global_index));
        
        fscanf(f, "%d", &(motif_node->motif_type));
        fscanf(f, "%d", &(motif_node->initial_global_index));
        
        motif_node->in_lz = 0;
        motif_node->lz_index = -1;
        motif_node->lz = NULL;

        motif_node->next_motif = NULL;
    }

#ifdef DEBUG3
    // print motifs information
    motif_node = ind->motifs_new;
    printf(" > > Printing motifs:\n");
    for(i = 0; i<ind->n_motifs; i++){
        //printf(" > > > Motif type %"PRIu8", first neuron index %"PRIu32"\n", motif_node->motif_type, motif_node->initial_global_index);
        printf(" > > > Motif type %d, first neuron index %d\n", motif_node->motif_type, motif_node->initial_global_index);
        motif_node = motif_node->next_motif;
    }
    fflush(stdout);
#endif

    // load neurons
    ind->neurons = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
    neuron_node_t *neuron_node = ind->neurons;

    fscanf(f, "%lf", &(neuron_node->threshold));
    fscanf(f, "%lf", &(neuron_node->v_rest));
    //fscanf(f, "%"SCNu8, &(neuron_node->refract_time));
    fscanf(f, "%d", &(neuron_node->refract_time));
    fscanf(f, "%f", &(neuron_node->r));
    fscanf(f, "%d", &(neuron_node->behaviour));
    neuron_node->next_neuron = NULL;

    for(i = 1; i<ind->n_neurons; i++){
        neuron_node->next_neuron = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
        neuron_node = neuron_node->next_neuron;

        fscanf(f, "%lf", &(neuron_node->threshold));
        fscanf(f, "%lf", &(neuron_node->v_rest));
        //fscanf(f, "%"SCNu8, &(neuron_node->refract_time));
        fscanf(f, "%d", &(neuron_node->refract_time));
        fscanf(f, "%f", &(neuron_node->r));
        fscanf(f, "%d", &(neuron_node->behaviour));
        
        neuron_node->next_neuron = NULL;
    }

#ifdef DEBUG3
    // print motifs information
    neuron_node = ind->neurons;
    printf(" > > Printing neurons:\n");
    for(i = 0; i<ind->n_neurons; i++){
        //printf(" > > > V.t %lf, v.r %lf, r.t %"PRIu8", R %f, behav %d\n", neuron_node->threshold, neuron_node->v_rest, neuron_node->refract_time, neuron_node->r, neuron_node->behaviour);
        printf(" > > > V.t %lf, v.r %lf, r.t %d, R %f, behav %d\n", neuron_node->threshold, neuron_node->v_rest, neuron_node->refract_time, neuron_node->r, neuron_node->behaviour);        
        neuron_node = neuron_node->next_neuron;
    }
    fflush(stdout);
#endif

    // connect the motifs and the neurons
    connect_motifs_and_neurons(ind);

    // load dynamic lists of connectivity
    int_node_t *int_node, *prev_int_node;

    ind->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));
    ind->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));

    for(i = 0; i<ind->n_motifs; i++){
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

#ifdef DEBUG3
    // print connectivity data
    printf(" > > Printing connectivity:\n");
    for(i = 0; i<ind->n_motifs; i++){

        printf(" > > > N input and output nodes: %d, %d\n", 
                ind->connectivity_info.in_connections[i].n_nodes, ind->connectivity_info.out_connections[i].n_nodes);
        
        int_node = ind->connectivity_info.in_connections[i].first_node;
        printf(" > > > > Input: ");
        while(int_node){
            printf("%d ", int_node->value);
            int_node = int_node->next;
        }
        printf("\n");
        
        int_node = ind->connectivity_info.out_connections[i].first_node;
        printf(" > > > > Output: ");
        while(int_node){
            printf("%d ", int_node->value);
            int_node = int_node->next;
        }
        printf("\n\n");
    }
    fflush(stdout);
#endif
    
    
    // load synapses
    ind->connectivity_matrix = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    //fscanf(f, "%"SCNu32, &(synapse_node->row));
    //fscanf(f, "%"SCNu32, &(synapse_node->col));
    //fscanf(f, "%"SCNd8, &(synapse_node->value));
    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));

    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
    synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

    i = 0;
    while(i < abs(synapse_node->value)){
        fscanf(f, "%lf", &(synapse_node->weight[i]));
        //fscanf(f, "%"SCNu8, &(synapse_node->latency[i]));
        //fscanf(f, "%"SCNu8, &(synapse_node->learning_rule[i]));
        fscanf(f, "%d", &(synapse_node->latency[i]));
        fscanf(f, "%d", &(synapse_node->learning_rule[i]));

        i++;
    }
    synapse_node->next_element = NULL;

    while(i<ind->n_synapses-ind->n_input_synapses){
        
        synapse_node->next_element = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
        synapse_node = synapse_node->next_element;

        //fscanf(f, "%"SCNu32, &(synapse_node->row));
        //fscanf(f, "%"SCNu32, &(synapse_node->col));
        //fscanf(f, "%"SCNd8, &(synapse_node->value));
        fscanf(f, "%d", &(synapse_node->row));
        fscanf(f, "%d", &(synapse_node->col));
        fscanf(f, "%d", &(synapse_node->value));
        
        synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
        synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

        for(j = 0; j<abs(synapse_node->value); j++){

            fscanf(f, "%lf", &(synapse_node->weight[j]));
            //fscanf(f, "%"SCNu8, &(synapse_node->latency[j]));
            //fscanf(f, "%"SCNu8, &(synapse_node->learning_rule[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));

            i++;
        }
        synapse_node->next_element = NULL;
    }

#ifdef DEBUG3
    // print synapses
    synapse_node = ind->connectivity_matrix;
    while(synapse_node){
        //printf(" > > > > row %"PRIu32", col %"PRIu32", value %"PRId8"\n", synapse_node->row, synapse_node->col, synapse_node->value);
        printf(" > > > > row %d, col %d, value %d\n", synapse_node->row, synapse_node->col, synapse_node->value);

        for(i = 0; i<abs(synapse_node->value); i++){
            //printf(" > > > > > w %lf, latency %"PRIu8", lr %"PRIu8"\n", synapse_node->weight[i], synapse_node->latency[i], synapse_node->learning_rule[i]);
            printf(" > > > > > w %lf, latency %d, lr %d\n", synapse_node->weight[i], synapse_node->latency[i], synapse_node->learning_rule[i]);
        }
        printf("\n");

        synapse_node = synapse_node->next_element;
    }
    printf("\n");
    fflush(stdout);
#endif
    
    // load input synapses
    ind->input_synapses = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    synapse_node = ind->input_synapses;

    //fscanf(f, "%"SCNu32, &(synapse_node->row));
    //fscanf(f, "%"SCNu32, &(synapse_node->col));
    //fscanf(f, "%"SCNd8, &(synapse_node->value));
    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));
    
    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
    synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

    i = 0;
    while(i < abs(synapse_node->value)){

        fscanf(f, "%lf", &(synapse_node->weight[i]));
        //fscanf(f, "%"SCNu8, &(synapse_node->latency[i]));
        //fscanf(f, "%"SCNu8, &(synapse_node->learning_rule[i]));
        fscanf(f, "%d", &(synapse_node->latency[i]));
        fscanf(f, "%d", &(synapse_node->learning_rule[i]));
        i++;
    }
    synapse_node->next_element = NULL;

    while(i<ind->n_input_synapses){
        synapse_node->next_element = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
        synapse_node = synapse_node->next_element;
        
        //fscanf(f, "%"SCNu32, &(synapse_node->row));
        //fscanf(f, "%"SCNu32, &(synapse_node->col));
        //fscanf(f, "%"SCNd8, &(synapse_node->value));
        fscanf(f, "%d", &(synapse_node->row));
        fscanf(f, "%d", &(synapse_node->col));
        fscanf(f, "%d", &(synapse_node->value));
        
        synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
        synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        
        for(j = 0; j<abs(synapse_node->value); j++){
        
            fscanf(f, "%lf", &(synapse_node->weight[j]));
            //fscanf(f, "%"SCNu8, &(synapse_node->latency[j]));
            //fscanf(f, "%"SCNu8, &(synapse_node->learning_rule[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));
            i++;
        }
        synapse_node->next_element = NULL;
    }

#ifdef DEBUG3
    // print input syanapses
    synapse_node = ind->input_synapses;
    while(synapse_node){
        printf(" > > > > row %d, col %d, value %d\n", synapse_node->row, synapse_node->col, synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            printf(" > > > > > w %lf, latency %d, lr %d\n", synapse_node->weight[i], synapse_node->latency[i], synapse_node->learning_rule[i]);
        }
        printf("\n");

        synapse_node = synapse_node->next_element;
    }
    printf("\n");
    fflush(stdout);
#endif

    // there are no learning zones
    ind->learning_zones = NULL;

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


    // store motifs
    new_motif_t *motif_node = ind->motifs_new;
    while(motif_node){
        fprintf(f, "%d", motif_node->motif_type);
        fprintf(f, "%d", motif_node->initial_global_index);
        fprintf(f, "\n");
        motif_node = motif_node->next_motif;
    }
    fprintf(f, "\n");

    // store neurons
    neuron_node_t *neuron_node = ind->neurons;
    while(neuron_node){
        fprintf(f, "%lf ", neuron_node->threshold);
        fprintf(f, "%lf ", neuron_node->v_rest);
        fprintf(f, "%d", neuron_node->refract_time);
        fprintf(f, "%f ", neuron_node->r);
        fprintf(f, "%d ", neuron_node->behaviour);
        fprintf(f , "\n");
        neuron_node = neuron_node->next_neuron;
    }
    fprintf(f, "\n");


    // store dynamic lists of connectivity
    int_node_t *int_node;
    for(i = 0; i<ind->n_motifs; i++){
        fprintf(f, "%d ", ind->connectivity_info.in_connections[i].n_nodes);
        fprintf(f, "%d\n", ind->connectivity_info.out_connections[i].n_nodes);
    
        // store input nodes
        int_node = ind->connectivity_info.in_connections[i].first_node;

        for(j = 0; j<ind->connectivity_info.in_connections[i].n_nodes; j++){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n");

        int_node = ind->connectivity_info.out_connections[i].first_node;

        for(j = 0; j<ind->connectivity_info.out_connections[i].n_nodes; j++){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n\n");
    }


    // store synapses
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    while(synapse_node){
        fprintf(f, "%d", synapse_node->row);
        fprintf(f, "%d", synapse_node->col);
        fprintf(f, "%d", synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            fprintf(f, "%lf ", synapse_node->weight[i]);
            fprintf(f, "%d", synapse_node->latency[i]);
            fprintf(f, "%d", synapse_node->learning_rule[i]);
        }
        fprintf(f, "\n");
        synapse_node = synapse_node->next_element;
    }

    fprintf(f, "\n");
    
    // store input synapses
    synapse_node = ind->input_synapses;

    while(synapse_node){
        fprintf(f, "%d", synapse_node->row);
        fprintf(f, "%d", synapse_node->col);
        fprintf(f, "%d", synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            fprintf(f, "%lf ", synapse_node->weight[i]);
            fprintf(f, "%d", synapse_node->latency[i]);
            fprintf(f, "%d", synapse_node->learning_rule[i]);
        }
        fprintf(f, "\n");
        synapse_node = synapse_node->next_element;
    }

    fprintf(f, "\n");

#ifdef DEBUG1
    motif_node = ind->motifs_new;
    int counter = 0;
    while(motif_node){
        counter ++;
        motif_node = motif_node->next_motif;
    }

    printf(" > > n_motifs = %d, counter = %d\n", ind->n_motifs, counter);
    fflush(stdout);

    neuron_node = ind->neurons;
    counter = 0;
    while(neuron_node){
        counter ++;
        neuron_node = neuron_node->next_neuron;
    }

    printf(" > > n_neurons = %d, counter = %d\n", ind->n_neurons, counter);
    fflush(stdout);


    synapse_node = ind->connectivity_matrix;
    counter = 0;
    while(synapse_node){
        counter += abs(synapse_node->value);
        synapse_node = synapse_node->next_element;
    }

    printf(" > > n_synapses = %d, counter = %d\n", ind->n_synapses - ind->n_input_synapses, counter);
    fflush(stdout);
#endif

    fflush(f);
}

void load_individual_from_file_learning_zone_included(FILE *f, individual *ind){
    
    int i, j;

    // load general information
    fscanf(f, "%d", &(ind->n_motifs));
    fscanf(f, "%d", &(ind->n_neurons));
    fscanf(f, "%d", &(ind->n_input_neurons));
    fscanf(f, "%d", &(ind->n_synapses));
    fscanf(f, "%d", &(ind->n_input_synapses));
    fscanf(f, "%d", &(ind->n_learning_zones));
    printf("General information loaded\n");
    fflush(stdout);

#ifdef DEBUG3
    // print general information
    printf(" > > General information: n_motifs %d, n_neurons %d, n_input_neurons %d, n_synapses %d, n_input_synapses %d, n_lz %d\n",
            ind->n_motifs, ind->n_neurons, ind->n_input_neurons, ind->n_synapses, ind->n_input_synapses, ind->n_learning_zones);
    fflush(stdout);
#endif


    // load motifs
    ind->motifs_new = (new_motif_t *)calloc(1, sizeof(new_motif_t));
    new_motif_t *motif_node = ind->motifs_new;

    fscanf(f, "%d", &(motif_node->motif_type));
    fscanf(f, "%d", &(motif_node->initial_global_index));
    motif_node->in_lz = 0;
    motif_node->lz_index = -1;
    motif_node->lz = NULL;

    motif_node->next_motif = NULL;

    for(i = 1; i<ind->n_motifs; i++){
        motif_node->next_motif = (new_motif_t *)calloc(1, sizeof(new_motif_t));
        motif_node = motif_node->next_motif;

        fscanf(f, "%d", &(motif_node->motif_type));
        fscanf(f, "%d", &(motif_node->initial_global_index));
        fscanf(f, "%d", &(motif_node->lz_index));
        motif_node->in_lz = 0;
        motif_node->lz = NULL;

        motif_node->next_motif = NULL;
    }

#ifdef DEBUG3
    // print motifs information
    motif_node = ind->motifs_new;
    printf(" > > Printing motifs:\n");
    for(i = 0; i<ind->n_motifs; i++){
        printf(" > > > Motif type %d, first neuron index %d\n", motif_node->motif_type, motif_node->initial_global_index);
        motif_node = motif_node->next_motif;
    }
    fflush(stdout);
#endif

    // load neurons
    ind->neurons = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
    neuron_node_t *neuron_node = ind->neurons;

    fscanf(f, "%lf", &(neuron_node->threshold));
    fscanf(f, "%lf", &(neuron_node->v_rest));
    fscanf(f, "%d", &(neuron_node->refract_time));
    fscanf(f, "%f", &(neuron_node->r));
    fscanf(f, "%d", &(neuron_node->behaviour));
    neuron_node->next_neuron = NULL;

    for(i = 1; i<ind->n_neurons; i++){
        neuron_node->next_neuron = (neuron_node_t *)calloc(1, sizeof(neuron_node_t));
        neuron_node = neuron_node->next_neuron;

        fscanf(f, "%lf", &(neuron_node->threshold));
        fscanf(f, "%lf", &(neuron_node->v_rest));
        fscanf(f, "%d", &(neuron_node->refract_time));
        fscanf(f, "%f", &(neuron_node->r));
        fscanf(f, "%d", &(neuron_node->behaviour));
        
        neuron_node->next_neuron = NULL;
    }

#ifdef DEBUG3
    // print motifs information
    neuron_node = ind->neurons;
    printf(" > > Printing neurons:\n");
    for(i = 0; i<ind->n_neurons; i++){
        printf(" > > > V.t %lf, v.r %lf, r.t %d, R %f, behav %d\n", neuron_node->threshold, neuron_node->v_rest, neuron_node->refract_time, neuron_node->r, neuron_node->behaviour);
        neuron_node = neuron_node->next_neuron;
    }
    fflush(stdout);
#endif

    // connect the motifs and the neurons
    connect_motifs_and_neurons(ind);

    // load dynamic lists of connectivity
    int_node_t *int_node, *prev_int_node;

    ind->connectivity_info.in_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));
    ind->connectivity_info.out_connections = (int_dynamic_list_t *)calloc(ind->n_motifs, sizeof(int_dynamic_list_t));

    for(i = 0; i<ind->n_motifs; i++){
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

#ifdef DEBUG3
    // print connectivity data
    printf(" > > Printing connectivity:\n");
    for(i = 0; i<ind->n_motifs; i++){

        printf(" > > > N input and output nodes: %d, %d\n", 
                ind->connectivity_info.in_connections[i].n_nodes, ind->connectivity_info.out_connections[i].n_nodes);
        
        int_node = ind->connectivity_info.in_connections[i].first_node;
        printf(" > > > > Input: ");
        while(int_node){
            printf("%d ", int_node->value);
            int_node = int_node->next;
        }
        printf("\n");
        
        int_node = ind->connectivity_info.out_connections[i].first_node;
        printf(" > > > > Output: ");
        while(int_node){
            printf("%d ", int_node->value);
            int_node = int_node->next;
        }
        printf("\n\n");
    }
    fflush(stdout);
#endif
    
    
    // load synapses
    ind->connectivity_matrix = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));

    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
    synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

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
        synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

        for(j = 0; j<abs(synapse_node->value); j++){

            fscanf(f, "%lf", &(synapse_node->weight[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));

            i++;
        }
        synapse_node->next_element = NULL;
    }

#ifdef DEBUG3
    // print synapses
    synapse_node = ind->connectivity_matrix;
    while(synapse_node){
        printf(" > > > > row %d, col %d, value %d\n", synapse_node->row, synapse_node->col, synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            printf(" > > > > > w %lf, latency %d, lr %d\n", synapse_node->weight[i], synapse_node->latency[i], synapse_node->learning_rule[i]);
        }
        printf("\n");

        synapse_node = synapse_node->next_element;
    }
    printf("\n");
    fflush(stdout);
#endif
    
    // load input synapses
    ind->input_synapses = (sparse_matrix_node_t *)calloc(1, sizeof(sparse_matrix_node_t));
    synapse_node = ind->input_synapses;

    fscanf(f, "%d", &(synapse_node->row));
    fscanf(f, "%d", &(synapse_node->col));
    fscanf(f, "%d", &(synapse_node->value));
    
    synapse_node->weight = (double *)malloc(abs(synapse_node->value) * sizeof(double));
    synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
    synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));

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
        synapse_node->latency = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        synapse_node->learning_rule = (int *)malloc(abs(synapse_node->value) * sizeof(int));
        
        for(j = 0; j<abs(synapse_node->value); j++){
        
            fscanf(f, "%lf", &(synapse_node->weight[j]));
            fscanf(f, "%d", &(synapse_node->latency[j]));
            fscanf(f, "%d", &(synapse_node->learning_rule[j]));
            i++;
        }
        synapse_node->next_element = NULL;
    }

#ifdef DEBUG3
    // print input syanapses
    synapse_node = ind->input_synapses;
    while(synapse_node){
        printf(" > > > > row %d, col %d, value %d\n", synapse_node->row, synapse_node->col, synapse_node->value);
        for(i = 0; i<abs(synapse_node->value); i++){
            printf(" > > > > > w %lf, latency %d, lr %d\n", synapse_node->weight[i], synapse_node->latency[i], synapse_node->learning_rule[i]);
        }
        printf("\n");

        synapse_node = synapse_node->next_element;
    }
    printf("\n");
    fflush(stdout);
#endif


    // load learning zones - load the first one
    learning_zone_t *lz, *p_lz;
    ind->learning_zones = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));
    lz = ind->learning_zones;

    fscanf(f, "%d", &(lz->n_lr));
    lz->lr = (int *)calloc(lz->n_lr, sizeof(int));
    for(i = 0; i<lz->n_lr; i++){
        fscanf(f, "%d", &(lz->lr[i]));
    }
    fscanf(f, "%d", &(lz->n_motifs));
    lz->previous_zone = NULL;
    lz->next_zone = NULL;

    // load the rest
    for(i = 1; i<ind->n_learning_zones; i++){
        
        p_lz = lz;
        lz = (learning_zone_t *)calloc(1, sizeof(learning_zone_t));

        fscanf(f, "%d", &(lz->n_lr));
        lz->lr = (int *)calloc(lz->n_lr, sizeof(int));
        for(j = 0; j<lz->n_lr; j++){
            fscanf(f, "%d", &(lz->lr[j]));
        }
        fscanf(f, "%d", &(lz->n_motifs));

        lz->next_zone = NULL;
        lz->previous_zone = p_lz;
        p_lz->next_zone = lz;
    }


    // connect learning zones and motifs
    new_motif_t **motifs_array = (new_motif_t **)calloc(ind->n_motifs, sizeof(new_motif_t *));
    learning_zone_t **lz_array = (learning_zone_t **)calloc(ind->n_learning_zones, sizeof(learning_zone_t *));

    // copy motifs and learning zones to helper arrays
    motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        motifs_array[i] = motif_node;
        motif_node = motif_node->next_motif;
    }

    lz = ind->learning_zones;
    for(i = 0; i<ind->n_learning_zones; i++){
        lz_array[i] = lz;
        lz = lz->next_zone;
    }

    // connect motifs and learning zones
    for(i = 0; i<ind->n_motifs; i++){
        motifs_array[i]->lz = lz_array[motifs_array[i]->lz_index];
    }


    fflush(stdout);
}


void store_individual_in_file_learning_zone_included(FILE *f, individual *ind){
    
    int i, j;

    // store general information
    fprintf(f, "%d\n", ind->n_motifs);
    fprintf(f, "%d\n", ind->n_neurons);
    fprintf(f, "%d\n", ind->n_input_neurons);
    fprintf(f, "%d\n", ind->n_synapses);
    fprintf(f, "%d\n", ind->n_input_synapses);
    fprintf(f, "%d\n", ind->n_learning_zones);
    fprintf(f, "\n");

    // store motifs
    new_motif_t *motif_node = ind->motifs_new;
    while(motif_node){
        fprintf(f, "%d ", motif_node->motif_type);
        fprintf(f, "%d ", motif_node->initial_global_index);
        fprintf(f, "%d ", motif_node->lz_index);
        fprintf(f, "\n");
        motif_node = motif_node->next_motif;
    }
    fprintf(f, "\n");

    // store neurons
    neuron_node_t *neuron_node = ind->neurons;
    while(neuron_node){
        fprintf(f, "%lf ", neuron_node->threshold);
        fprintf(f, "%lf ", neuron_node->v_rest);
        fprintf(f, "%d ", neuron_node->refract_time);
        fprintf(f, "%f ", neuron_node->r);
        fprintf(f, "%d ", neuron_node->behaviour);
        fprintf(f , "\n");
        neuron_node = neuron_node->next_neuron;
    }
    fprintf(f, "\n");


    // store dynamic lists of connectivity
    int_node_t *int_node;
    for(i = 0; i<ind->n_motifs; i++){
        fprintf(f, "%d ", ind->connectivity_info.in_connections[i].n_nodes);
        fprintf(f, "%d\n", ind->connectivity_info.out_connections[i].n_nodes);
    
        // store input nodes
        int_node = ind->connectivity_info.in_connections[i].first_node;

        for(j = 0; j<ind->connectivity_info.in_connections[i].n_nodes; j++){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n");

        int_node = ind->connectivity_info.out_connections[i].first_node;

        for(j = 0; j<ind->connectivity_info.out_connections[i].n_nodes; j++){
            fprintf(f, "%d ", int_node->value);
            int_node = int_node->next;
        }
        fprintf(f, "\n\n");
    }


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

    // store learning zones
    learning_zone_t *lz;
    lz = ind->learning_zones;
    for(i = 0; i<ind->n_learning_zones; i++){

        fprintf(f, "%d ", lz->n_motifs);
        fprintf(f, "%d ", lz->n_lr);
        for(j = 0; j<lz->n_lr; j++){
            fprintf(f, "%d ", lz->lr[j]);
        }
        fprintf(f, "\n");

        lz = lz->next_zone;
    }
    fprintf(f, "\n");
    

    fflush(f);
}


/**
 * Helpers
 */
void get_complete_matrix_from_dynamic_list(int *complete_matrix, sparse_matrix_node_t *matrix_node, int n_neurons){

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