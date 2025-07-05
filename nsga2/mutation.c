/* Mutation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"

// TODO: DEALLOCATE MEMORY

/* Function to perform mutation in a population */
void mutation_pop (NSGA2Type *nsga2Params,  population *pop)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        mutation_ind(nsga2Params, &(pop->ind[i]));
    }
    return;
}


// MUTATE ALWAYS
/* Function to perform mutation of an individual */
void mutation_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    /*
    Existing mutations:
        - Add motif(s)
        - Remove motif(s)
        - Connect motif
        - Disconnect motif(s)
        - Change refractory time value(s)
        - CHange threshold
        - Change v rest
        - Change latency
    */

    int mutation_code = rnd(0, 6); // the code indicates what mutation will be done
    int mutation_parameter;

    // this should be refactorized
    switch(mutation_code){
        case 0: // change threshold
            mutation_parameter = rnd(0, 2);
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 1: // change v_rest
            mutation_parameter = rnd(0, 0);
            synapse_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 2: // change latency
            mutation_parameter =  rnd(1, 3); // TODO: n_motifs, this should not be a fixed value
            add_motif_mutation(nsga2Params, ind, mutation_parameter);
            break;
        case 3: // change refract time
            mutation_parameter =  rnd(1, 3); // TODO: n_motifs, this should not be a fixed value
            remove_motif_mutation(nsga2Params, ind, mutation_parameter);
            break;
        case 4: // add motif
            break;
        case 5: // remove motif
            break;
    }

    return;
}

/* Select randomly what neurons will be mutated */
int_array_t* select_neurons_to_change(individual *ind){
    int i;

    int_array_t *selected_neurons;
    selected_neurons = (int_array_t *)malloc(sizeof(int_array_t));
    selected_neurons->n = rnd(1, 1); // for now only one
    selected_neurons->array = malloc(selected_neurons->n * sizeof(int));

    // select neurons to change values
    for(i = 0; i<selected_neurons->n; i++)
        selected_neurons->array[i] = rnd(0, ind->n_neurons - 1);

    // order the list
    insertion_sort(selected_neurons->array, selected_neurons->n);

    // return the list
    return selected_neurons;
}

/* Select randomly what synapses will be mutated */
int_array_t* select_synapses_to_change(individual *ind){
    int i;

    int_array_t *selected_synapses;
    selected_synapses = (int_array_t *)malloc(sizeof(int_array_t));
    selected_synapses->n = rnd(1, 1); // for now only one
    selected_synapses->array = malloc(selected_synapses->n * sizeof(int));

    // select neurons to change values
    for(i = 0; i<selected_synapses->n; i++)
        selected_synapses->array[i] = rnd(0, ind->n_neurons - 1);

    // order the list
    insertion_sort(selected_synapses->array, selected_synapses->n);

    // return the list
    return selected_synapses;
}

/* This mutation changes something on the selected neurons depending on the mutation code */
void neuron_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code){
    // select neurons to change
    int i, j = 0;

    // select neuron to do the changes
    int_array_t *selected_neurons = select_neurons_to_change(ind);
    
    // loop over dynamic list of neurons to find the selected ones and change them
    neuron_node_t *neuron_node = ind->neurons;
    for(i = 0; i<selected_neurons->n; i++){
        // loop until we reach the selected neuron node
        while(j != selected_neurons->array[i]){
            neuron_node = neuron_node->next_neuron;
            j++;
        }

        // change neuron property value depending on mutation type
        switch(mutation_code){
            case 0: // change threshold
                neuron_node->threshold = neuron_node->threshold + (rndreal(-neuron_node->threshold * 0.25, neuron_node->threshold * 0.25));
                break;
            case 1: // change v_rest
                neuron_node->v_rest = neuron_node->v_rest + (rnd(-2, 2));
                if(neuron_node->v_rest < 0)
                    neuron_node->v_rest = 0;
                break;
            case 2: // change refract time
                neuron_node->refract_time = neuron_node->refract_time + (rnd(-2, 2));
                if(neuron_node->refract_time < 0)
                    neuron_node->refract_time = 0;
                break;
        }        
        neuron_node->threshold = neuron_node->threshold + (rndreal(-neuron_node->threshold * 0.25, neuron_node->threshold * 0.25));
        neuron_node = neuron_node->next_neuron;
        j++;
    }
}

/* This mutation changes something on the selected synapses depending on the mutation code */
void synapse_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code){
    // select neurons to change
    int i, j = 0;

    // select neuron to do the changes
    int_array_t *selected_synapses = select_synapses_to_change(ind);
    
    // loop over dynamic list of neurons to find the selected ones and change them
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;
    for(i = 0; i<selected_synapses->n; i++){
        // loop until we reach the selected neuron node
        while(j != selected_synapses->array[i]){
            synapse_node = synapse_node->next_element;
            j++;
        }

        // change neuron property value depending on mutation type
        switch(mutation_code){
            case 0: // change threshold
                synapse_node->latency = synapse_node->latency + (rnd(-2, 2));
                if(synapse_node->latency <= 0)
                    synapse_node->latency = 1;
                break;
        }        
        synapse_node = synapse_node->next_element;
        j++;
    }
}

/* Select randomly what motifs will be mutated */
int_array_t* select_motifs(individual *ind, int n_motifs, int min_connected_motifs, int max_connected_motifs){
    int i, j;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(n_motifs * sizeof(int_array_t)); // selected input/output motifs per each new motif
    

    // per each new motif, select input or output motifs
    for(i = 0; i<n_motifs; i++){
        // select amount of motifs and allocate memory
        selected_motifs[i].n = rnd(min_connected_motifs, max_connected_motifs); // TODO: this should not be 1 always
        selected_motifs[i].array = malloc(selected_motifs[i].n * sizeof(int));

        for(j = 0; j<selected_motifs[i].n; j++){
            selected_motifs[i].array[j] = rnd(0, ind->n_motifs); // the new motif can be connected with itself, so not -1
        }
        // order the list
        insertion_sort(selected_motifs[i].array, selected_motifs[i].n);
    }
        
    // return the list
    return selected_motifs;
}

/* Select randomly what motifs will be removed */
int_array_t* select_motifs_to_be_removed(individual *ind, int n_motifs){
    int i, j, selected = 0, eq = 1;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(sizeof(int_array_t)); // selected input/output motifs per each new motif
    

    // select motifs to be removed. A motif can not be appear two times
    selected_motifs->n = n_motifs; // TODO: this should not be 1 always
    selected_motifs->array = malloc(selected_motifs->n * sizeof(int));

    while(selected < selected_motifs->n){
        selected_motifs->array[selected] = rnd(0, ind->n_motifs);

        eq = 1;
        for(i = 0; i<selected; i++){
            if(selected_motifs->array[i] == selected_motifs->array[selected])
                eq = 0;
        }

        // if it was already selected, repeat select another motif for the same position
        selected += eq;
    }

    insertion_sort(selected_motifs->array, selected_motifs->n);
        
    // return the list
    return selected_motifs;
}


/* This mutation adds a new motif (or more than one) to the network (with random connections) */
void add_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs){
    int i = 0, j;

    int last_motif_id; // vairables to store the number of new motifs that are added to the genotype and the id of the last processed motif
    new_motif_t *motif_node; // motif nodes to store the new motifs and to loop over the dynamic list of motifs
    neuron_node_t *neuron_node; // neuron node to loop over the dynamic list of neurons and to add new ones
    int min_connected_motifs, max_connected_motifs;
    int_array_t *selected_input_motifs, *selected_output_motifs, *all_selected_input_motifs; // lists to store the selected input and output motifs for the new motif

    // add the number of new motifs to the individual
    ind->n_motifs += n_new_motifs;

    // get the first motif and synapse
    motif_node = ind->motifs_new; // first motif
    neuron_node = ind->neurons;

    // loop over motifs and locate the new one at the last position
    /*for(i = 0; i<ind->n_motifs-1; i++)
        tmp_motif_node = tmp_motif_node->next_motif; // loop until the last motif*/

    // loop until the last motif is acquited
    while(motif_node->next_motif != NULL)
        motif_node = motif_node->next_motif;

    // store last motif id, as the new motif id will be the following one
    last_motif_id = motif_node->motif_id;

    // loop over the network neurons until the last is reached
    /*for(i = 0; i<ind->n_neurons-1; i++)
        neuron_node = neuron_node->next_neuron;*/
    
    // loop over the neurons until the last is reached
    while(neuron_node->next_neuron != NULL)
        neuron_node = neuron_node->next_neuron;

    // add new motifs, and for each motif, its neurons, to the dynamic lists
    for(i = 0; i<n_new_motifs; i++){
        // update motif id for the next one
        last_motif_id ++;

        // allocate memory for the next motif (the new one)
        motif_node = initialize_and_allocate_motif(motif_node, last_motif_id, ind);

        // add motif neurons to the dynamic list of neurons
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++)
            initialize_and_allocate_neuron_node(neuron_node);
    }

    /* Select input and output motifs for each new motif, and then map all to*/
    min_connected_motifs = 1;
    max_connected_motifs = 3;
    selected_input_motifs = select_motifs(ind, n_new_motifs, min_connected_motifs ,max_connected_motifs); // select input motifs for each new motif
    selected_output_motifs = select_motifs(ind, n_new_motifs, min_connected_motifs, max_connected_motifs); // selected output motifs for each new motif
    all_selected_input_motifs = map_IO_motifs_to_input(ind, n_new_motifs, selected_input_motifs, selected_output_motifs); // map

    /* Add the new synaptic connectiosn to the connections list */
    update_sparse_matrix_add_motifs(ind, n_new_motifs, all_selected_input_motifs);
    
    // free memory
    deallocate_int_array(selected_input_motifs);
    deallocate_int_array(selected_output_motifs);
}


int_array_t* map_IO_motifs_to_input(individual *ind, int n_new_motifs, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs){
    int i, j;

    int_array_t *all_selected_input_motifs;
    int **aux_array, *n_aux;
    int tmp_motif_index;

    /* Allocate memory */
    all_selected_input_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));
    aux_array = (int **)malloc(ind->n_motifs * sizeof(int *));
    
    for(i = 0; i<ind->n_motifs; i++)
        aux_array[i] = (int *)malloc(ind->n_motifs * 3 * sizeof(int)); // TODO:the size of this should not be fixed
    n_aux = (int *)calloc(ind->n_motifs, sizeof(int));


    /* Map */

    // loop over new motifs for maping output motifs to input motifs 
    for(i = 0; i<n_new_motifs; i++){
        // loop over output motifs of this motif
        for(j = 0; j<selected_output_motifs[i].n; j++){
            tmp_motif_index = selected_output_motifs[i].array[j];
            aux_array[tmp_motif_index][n_aux[tmp_motif_index]] = i + ind->n_motifs - n_new_motifs; 
            n_aux[tmp_motif_index] ++;
        }

        // loop over input motifs to add to the previous array
        for(j=0; j<selected_input_motifs[i].n; j++){
            tmp_motif_index = i + ind->n_motifs - n_new_motifs;
            aux_array[tmp_motif_index][n_aux[tmp_motif_index]] = selected_input_motifs[i].array[j]; 
            n_aux[tmp_motif_index] ++;
        }
    }


    /* Copy */

    // loop over old motifs and copy the new input motifs
    for(i = 0; i<ind->n_motifs; i++){
        all_selected_input_motifs[i].n = n_aux[i];
        all_selected_input_motifs[i].array = (int *)malloc(n_aux[i] * sizeof(int));

        for(j = 0; j<n_aux[i]; j++){
            all_selected_input_motifs[i].array[j] = aux_array[i][j];
        }

        insertion_sort(all_selected_input_motifs[i].array, all_selected_input_motifs[i].n);
    }

    /* print lists */
/*    printf("\n");
    for( i = 0; i<ind->n_motifs; i++ ){
        for(j = 0; j<all_selected_input_motifs[i].n; j++){
            printf("%d ", all_selected_input_motifs[i].array[j]);
        }
        printf("\n");
    }
    printf("\n");
*/

    // free allocated memory
    free(n_aux);
    for(i = 0; i<ind->n_motifs; i++)
        free(aux_array[i]); 
    free(aux_array);

    return all_selected_input_motifs;
}

void update_sparse_matrix_add_motifs(individual *ind, int n_new_motifs, int_array_t *selected_input_motifs){
    int i, j;

    new_motif_t *motif_node;
    sparse_matrix_node_t *synapse_node, *next_column_synapse_node;
    sparse_matrix_build_info_t SMBI;

    // allocate memory for aux lists and initialize
    SMBI.motifs_types = (int *)malloc(ind->n_motifs * sizeof(int));
    SMBI.motifs_first_neuron_indexes = (int *)malloc(ind->n_motifs * sizeof(int));
    SMBI.actual_motif_proccessed = 1;

    // get motifs general information
    motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        SMBI.motifs_types[i] = motif_node->motif_type; // store the motif type
        SMBI.motifs_first_neuron_indexes[i] = motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        motif_node = motif_node->next_motif;
    }

    // loop over new motifs and add new columns of input motifs
    synapse_node = ind->connectivity_matrix;
    for(i = 0; i<ind->n_motifs - n_new_motifs; i++){
        // check if actual motif has new input motifs
        if(selected_input_motifs[i].n > 0){
        
            // store actual motif information
            SMBI.actual_motif_index = i;
            SMBI.actual_motif_type = SMBI.motifs_types[i];
            SMBI.actual_motif_first_neuron_index = SMBI.motifs_first_neuron_indexes[i];
            SMBI.actual_motif_info = &(motifs_data[SMBI.actual_motif_type]);
            
            // loop over actual motif neurons
            for(j = 0; j<SMBI.actual_motif_info->n_neurons; j++){
                SMBI.actual_neuron_local_index = j;

                // first check if we are in the correct column in the sparse matrix
                if(synapse_node->col == SMBI.actual_motif_first_neuron_index + j){
                    // go to the final of the column and add new input motifs if it is neccessary
                    // if it is NULL it means we reach the last column
                    while(synapse_node->next_element != NULL && synapse_node->col == synapse_node->next_element->col)
                        synapse_node = synapse_node->next_element;

                    // store next column first element to make the connection later
                    next_column_synapse_node = synapse_node->next_element; 

                    // add new cells to complete the column if the neuron is an output neuron
                    if(check_if_neuron_is_input(SMBI.actual_motif_type, j) == 1)
                        synapse_node = construct_neuron_sparse_matrix_column(ind, synapse_node, &(selected_input_motifs[i]), &SMBI);
                }

                // rconnect the last element newly added with the next column first element
                synapse_node->next_element = next_column_synapse_node;

                // move to the next column if it exists (if not, it means we finished processing old column and we will move to the second loop)
                if(synapse_node->next_element != NULL)  
                    synapse_node = synapse_node->next_element;
            }
        }
        else{
            // move to the next motif first column first element
            while(synapse_node->next_element != NULL && synapse_node->col < SMBI.motifs_first_neuron_indexes[i+1]) // next motif will never be NULL, new motifs added
                synapse_node = synapse_node->next_element;

        }
        
        // move to the next motif
        motif_node->next_motif;
    }


    // loop over new moitfs and add entire columns of input motifs
    for(i = ind->n_motifs - n_new_motifs; i < ind->n_motifs; i++){
        // store actual motif information
        SMBI.actual_motif_index = i;
        SMBI.actual_motif_type = SMBI.motifs_types[i];
        SMBI.actual_motif_first_neuron_index = SMBI.motifs_first_neuron_indexes[i];
        SMBI.actual_motif_info = &(motifs_data[SMBI.actual_motif_type]);
        
        // construct sparse matrix
        synapse_node = construct_motif_sparse_matrix_columns(ind, synapse_node, &(selected_input_motifs[i]), &SMBI);

        // move to the next motif
        motif_node->next_motif;
    }

    // set NULL after the last synapse of the sparse matrix
    synapse_node->next_element = NULL;

    // free allocated memory
    free(SMBI.motifs_types);
    free(SMBI.motifs_first_neuron_indexes);
}



void remove_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_remove_motifs){
    int i, j, s;
    new_motif_t *previous_motif_node, *tmp_motif_node, *motif_node;
    neuron_node_t *previous_neuron_node, *neuron_node, *tmp_neuron_node;
    motif_t *motif_data;
    int rest_neurons = 0;

    int_array_t* selected_motifs_to_remove = select_motifs_to_be_removed(ind, n_remove_motifs);

    // update sparse matrix removing neccessary synapses
    update_sparse_matrix_remove_motifs(ind, selected_motifs_to_remove);
    ind->n_motifs -= n_remove_motifs;

    // remove deleted motifs, and for each motif, its neurons, from the dynamic lists
    motif_node = ind->motifs_new;
    neuron_node = ind->neurons;

    i = 0;
    j = 0; 
    
    // loop until selected motifs are removed from the list
    while(j < selected_motifs_to_remove->n){

        tmp_motif_node = motif_node;
        tmp_motif_node->initial_global_index -= rest_neurons;
        motif_data = &(motifs_data[tmp_motif_node->motif_type]);

        motif_node = motif_node->next_motif;

        // check if the actual motif must be removed
        if(i == selected_motifs_to_remove->array[j]){

            // motif will be removed, so update 
            rest_neurons += motif_data->n_neurons;
            ind->n_neurons -= motif_data->n_neurons;

            // if the tmp motif is the first one, change the first motif of the list
            if(tmp_motif_node == ind->motifs_new){
                ind->motifs_new = motif_node;
            }
            // else, remove and connect previous with next one
            else{
                previous_motif_node->next_motif = motif_node;
            }
            
            // free memory of the motif node
            free(tmp_motif_node);

            // loop over motif neurons, remove them and connect the previous one with the first of the next motif
            for(s = 0; s<motif_data->n_neurons; s++){
                tmp_neuron_node = neuron_node;
                neuron_node = neuron_node->next_neuron;

                if(tmp_neuron_node == ind->neurons)
                    ind->neurons = neuron_node;
                else
                    previous_neuron_node->next_neuron = neuron_node;

                free(tmp_neuron_node);
            }


            // move to the next motif to be removed
            j++;
        }
        // update only previous motif node
        else{
            previous_motif_node = tmp_motif_node;

            // loop over motif neurons
            for(s = 0; s<motif_data->n_neurons; s++){
                previous_neuron_node = neuron_node;
                neuron_node = neuron_node->next_neuron;
            }
        }

        i++;
    }
}


void update_sparse_matrix_remove_motifs(individual *ind, int_array_t *selected_motifs){
    
    int i, j, s;
    int removed_motifs = 0, n_first_motifs;

    new_motif_t *motif_node;
    sparse_matrix_node_t *synapse_node, *tmp_synapse_node, *previous_synapse_node = NULL, *next_column_synapse_node;
    sparse_matrix_build_info_t SMBI;

    // allocate memory for aux lists and initialize
    SMBI.motifs_types = (int *)malloc(ind->n_motifs * sizeof(int));
    SMBI.motifs_first_neuron_indexes = (int *)malloc(ind->n_motifs * sizeof(int));

    // get motifs general information
    motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        SMBI.motifs_types[i] = motif_node->motif_type; // store the motif type
        SMBI.motifs_first_neuron_indexes[i] = motif_node->initial_global_index; // store the motif's first neuron global index

        // move to the next motif node
        motif_node = motif_node->next_motif;
    }


    // ============================================= //
    // Remove first columns of the sparse matrix if it is neccessary
    // ============================================= //

    // count how much of the first motifs are selected to be removed
    n_first_motifs = 0;
    while(selected_motifs->array[n_first_motifs] == n_first_motifs)
        n_first_motifs ++;

    synapse_node = ind->connectivity_matrix;


    // we are in the first synapse, so the previous one doesn't exists
    SMBI.previous_synapse_node = NULL; 


    // loop over all motifs, and check if that motif has cells to be removed
    for(i = 0; i<ind->n_motifs; i++){

        // store actual motif information
        SMBI.actual_motif_index = i;
        SMBI.actual_motif_type = SMBI.motifs_types[i];
        SMBI.actual_motif_first_neuron_index = SMBI.motifs_first_neuron_indexes[i];
        SMBI.actual_motif_info = &(motifs_data[SMBI.actual_motif_type]);
        

        // check if the actual motif must be removed (entire columns)
        int remove_actual_motif = 0;
        for(j = 0; j<selected_motifs->n; j++){
            if(selected_motifs->array[j] == SMBI.actual_motif_index)
                remove_actual_motif = 1; // actual motif must be removed
        }

    
        // if the motif must be removed, remove all synapses (all synapses of all nodes of the motif)
        if(remove_actual_motif > 0){       
            synapse_node = remove_all_motif_synapses(ind, &SMBI, synapse_node);
        }
        // else, remove only the cells of the selected motifs
        else{
            synapse_node = remove_selected_synapses(ind, &SMBI, selected_motifs, synapse_node);
        }
    }
}


sparse_matrix_node_t* remove_all_motif_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, sparse_matrix_node_t *synapse_node){
    
    sparse_matrix_node_t *tmp_synapse_node, *last_synapse_node;

    // store actual motif information
    int actual_motif_index = SMBI->actual_motif_index;
    int actual_motif_type = SMBI->actual_motif_type;
    int actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    motif_t *actual_motif_info = SMBI->actual_motif_info;

    // if the actual motif is the last one, remove until synapse node is NULL
    if(actual_motif_index == ind->n_motifs - 1){

        while(synapse_node != NULL){
            tmp_synapse_node = synapse_node;
            synapse_node = synapse_node->next_element;

            if(tmp_synapse_node == ind->connectivity_matrix)
                ind->connectivity_matrix = synapse_node;

            // free memory and update number of synapses
            ind->n_synapses -= abs(tmp_synapse_node->value);
            free(tmp_synapse_node);
        }

        SMBI->previous_synapse_node->next_element = NULL;
    }

    // else, loop until the next motif first synapse is reached
    else{
        while(synapse_node->col < SMBI->motifs_first_neuron_indexes[actual_motif_index+1]){
            tmp_synapse_node = synapse_node;
            synapse_node = synapse_node->next_element;

            if(tmp_synapse_node == ind->connectivity_matrix)
                ind->connectivity_matrix = synapse_node;

            // free memory and update number of synapses
            ind->n_synapses -= abs(tmp_synapse_node->value);
            free(tmp_synapse_node);
        }

        // once we are in the first column of the new motif, make the connection again between the previous motif and the actual one
        if(SMBI->previous_synapse_node != NULL)
            SMBI->previous_synapse_node->next_element = synapse_node;
    }

    return synapse_node;
}

sparse_matrix_node_t* remove_selected_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, int_array_t *selected_motifs, sparse_matrix_node_t *synapse_node){
    int i, j, motif_index, motif_type;
    motif_t *remove_motif_info;
    sparse_matrix_node_t *tmp_synapse_node;

    // store actual motif information
    int actual_motif_index = SMBI->actual_motif_index;
    int actual_motif_type = SMBI->actual_motif_type;
    int actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    motif_t *actual_motif_info = SMBI->actual_motif_info;
    motif_t *tmp_motif_info;
    int init_row, final_row;


    int rest_neuron_col = 0, rest_neuron_row = 0;


    // compute how much columns have been removed before the actual motif
    for(i = 0; i<selected_motifs->n; i++){
        if(selected_motifs->array[i] < actual_motif_index){
            rest_neuron_col += motifs_data[SMBI->motifs_types[selected_motifs->array[i]]].n_neurons;
        }
    }

    // loop over motif neurons to remove neccessary data from columns
    for(i = 0; i<actual_motif_info->n_neurons; i++){        
        
        // check that we are in the column of the actual neuron (first neuron index + local index)
        rest_neuron_row = 0;

        if(synapse_node && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){

            // loop over the selected motifs to be removed
            for(j = 0; j<selected_motifs->n; j++){

                motif_index = selected_motifs->array[j];
                motif_type = SMBI->motifs_types[motif_index];
                remove_motif_info = &(motifs_data[motif_type]);

                // the cell is previous to the motif that must be removed, so move to the next cell
                if(synapse_node && synapse_node->row < SMBI->motifs_first_neuron_indexes[motif_index]){

                    while(synapse_node->row < SMBI->motifs_first_neuron_indexes[motif_index] && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                        synapse_node->col -= rest_neuron_col;
                        synapse_node->row -= rest_neuron_row;

                        SMBI->previous_synapse_node = synapse_node; // store the actual synapse
                        synapse_node = synapse_node->next_element; // move to the next synapse
                    }
                }

                // remove the motif

                init_row = SMBI->motifs_first_neuron_indexes[motif_index];
                final_row = SMBI->motifs_first_neuron_indexes[motif_index] + remove_motif_info->n_neurons;
                
                if(synapse_node && synapse_node->row >= init_row && synapse_node->row < final_row){
                    
                    while(synapse_node && synapse_node != NULL && synapse_node->row >= init_row && synapse_node->row < final_row && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                        tmp_synapse_node = synapse_node;
                        synapse_node = synapse_node->next_element;

                        // if the tmp motif is the first one of the dynamic list, move it to the next
                        if(tmp_synapse_node == ind->connectivity_matrix)
                            ind->connectivity_matrix = synapse_node;

                        // free memory and update number of synapses
                        ind->n_synapses -= abs(tmp_synapse_node->value);
                        free(tmp_synapse_node);
                    }

                    // once we are in the first column of the new motif, make the connection again between the previous motif and the actual one
                    if(SMBI->previous_synapse_node != NULL)
                        SMBI->previous_synapse_node->next_element = synapse_node;
                }

                rest_neuron_row += remove_motif_info->n_neurons;
            }

            // if we are in the same column, move to the next as the computation of this column has already finished
            if(synapse_node && synapse_node->next_element != NULL && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                //printf(" > > > > Moving to the next column!\n");
                while(synapse_node && synapse_node->next_element != NULL && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                    synapse_node->col -= rest_neuron_col;
                    synapse_node->row -= rest_neuron_row;

                    SMBI->previous_synapse_node = synapse_node;
                    synapse_node = synapse_node->next_element;
                }

                // if it is the last element, update the column and the row
                if(synapse_node && synapse_node->next_element == NULL){
                    synapse_node->col -= rest_neuron_col;
                    synapse_node->row -= rest_neuron_row;
                }
            }
        }
    }
    
    return synapse_node;
}



/* This mutation adds new connections to the network */
void add_connection_mutation(NSGA2Type *nsga2Params, individual *ind, int n_connections){

}

/* Mutation to remove connections from the network */
void remove_connection_mutation(NSGA2Type *nsga2Params, individual *ind, int n_connections){

}



// THIS CAN BE REMOVED
/* Routine for binary mutation of an individual */
void bin_mutate_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    int j, k;
    double prob;
    for (j=0; j<nsga2Params->nbin; j++)
    {
        for (k=0; k<nsga2Params->nbits[j]; k++)
        {
            prob = randomperc();
            if (prob <=nsga2Params->pmut_bin)
            {
                if (ind->gene[j][k] == 0)
                {
                    ind->gene[j][k] = 1;
                }
                else
                {
                    ind->gene[j][k] = 0;
                }
                nsga2Params->nbinmut+=1;
            }
        }
    }
    return;
}


// THIS CAN BE REMOVED
/* Routine for real polynomial mutation of an individual */
void real_mutate_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    int j;
    double rnd, delta1, delta2, mut_pow, deltaq;
    double y, yl, yu, val, xy;
    for (j=0; j<nsga2Params->nreal; j++)
    {
        if (randomperc() <= nsga2Params->pmut_real)
        {
            y = ind->xreal[j];
            yl = nsga2Params->min_realvar[j];
            yu = nsga2Params->max_realvar[j];
            delta1 = (y-yl)/(yu-yl);
            delta2 = (yu-y)/(yu-yl);
            rnd = randomperc();
            mut_pow = 1.0/(nsga2Params->eta_m+1.0);
            if (rnd <= 0.5)
            {
                xy = 1.0-delta1;
                val = 2.0*rnd+(1.0-2.0*rnd)*(pow(xy,(nsga2Params->eta_m+1.0)));
                deltaq =  pow(val,mut_pow) - 1.0;
            }
            else
            {
                xy = 1.0-delta2;
                val = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,(nsga2Params->eta_m+1.0)));
                deltaq = 1.0 - (pow(val,mut_pow));
            }
            y = y + deltaq*(yu-yl);
            if (y<yl)
                y = yl;
            if (y>yu)
                y = yu;
            ind->xreal[j] = y;
            nsga2Params->nrealmut+=1;
        }
    }
    return;
}
