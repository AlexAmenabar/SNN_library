/* Mutation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"


/**
 * General mutation functions
 */

/* Function to perform mutation in a population */
void mutation_pop (NSGA2Type *nsga2Params,  population *pop)
{
    int i;
    for (i=0; i<nsga2Params->popsize; i++)
    {
        #ifdef DEBUG1
        printf(" > > Mutating individual %d...\n", i);
        fflush(stdout);
        #endif

        mutation_ind(nsga2Params, &(pop->ind[i]));
    }
    return;
}

/* Function to perform mutation of an individual */
void mutation_ind (NSGA2Type *nsga2Params,  individual *ind)
{
    int min,max;


#ifdef PHASE2

    int mutation_code = rnd(0,2);

    #ifdef DEBUG2
        printf(" > > > Mutation code %d\n", mutation_code);
        fflush(stdout);
    #endif

    
    switch (mutation_code)
    {
    case 0:
        add_learning_zone_mutation(nsga2Params, ind);
        break;
    case 1:
        extend_learning_zones_mutation(nsga2Params, ind);
        break;
    case 2:
        change_learning_rule_mutation(nsga2Params, ind);
        break;
    default:
        change_learning_rule_mutation(nsga2Params, ind);
        break;
    }

    // clear learning zones with 0 motifs
    int_array_t *r_lz = clear_learning_zones(nsga2Params, ind);

    // update learning zones indexes for motifs
    update_learning_zones_indexes(nsga2Params, ind, r_lz);

    // cp 
    // copy motifs from dynamic list to an array for efficiency
    new_motif_t **motifs_array, *motif_node;

    motifs_array = (new_motif_t **)calloc(ind->n_motifs, sizeof(new_motif_t *));
    motif_node = ind->motifs_new;

    // initialize helper arrays // TODO: Probably in this second phase the array of motifs can be helpful all the time, do it only once
    motif_node = ind->motifs_new;
    for(int i = 0; i<ind->n_motifs; i++){
        motifs_array[i] = motif_node;
        motif_node = motif_node->next_motif;
    }

    cp_learning_rules_in_synapses(nsga2Params, ind, motifs_array);

    free(motifs_array);
    free(r_lz->array);
    free(r_lz);

#else // phase 1

    // TODO: Change this
    int mutation_code = rnd(0, 3); // TODO: this should be more probabilistic
    int mutation_parameter;

    // this should be refactorized


    if(mutation_code == 3 && ind->n_motifs < nsga2Params->min_motifs) // when very few motifs are in the network, network can dissappear, this is used to avoid that
        mutation_code = 2; // This is temporal for testing

    #ifdef DEBUG1
        printf(" > > > Mutation code %d\n", mutation_code);
        fflush(stdout);
    #endif

    //printf(" > > Mutation code %d\n", mutation_code);
    switch(mutation_code){
        case 0: // mutate neuron(s) parameter(s)
            //mutation_parameter = rnd(0, 2);
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 1: // mutate synapse parameter(s)
            //mutation_parameter = rnd(0, 0);
            synapse_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 2: // add motif mutation
            min = (int)(ind->n_motifs * nsga2Params->min_motifs_add);
            max = (int)(ind->n_motifs * nsga2Params->max_motifs_add);

            if(min < 1) min = 1;
            if(max < 1) max = 1;

            mutation_parameter =  rnd(min, max);//rnd(1, 1); // TODO: n_motifs, this should not be a fixed value
                
            //mutation_parameter =  rnd(1, 5); // TODO: n_motifs, this should not be a fixed value
            add_motif_mutation(nsga2Params, ind, mutation_parameter, NULL, NULL, NULL);
            break;
        case 3: // remove motif mutation
            min = (int)(ind->n_motifs * nsga2Params->min_motifs_remove);
            max = (int)(ind->n_motifs * nsga2Params->max_motifs_remove);

            if(min < 1) min = 1;
            if(max < 1) max = 1;

            mutation_parameter =  rnd(min, max);//rnd(1, 1); // TODO: n_motifs, this should not be a fixed value
            #ifdef DEBUG2
            printf(" > > > > N motifs: %d / %d\n", mutation_parameter, ind->n_motifs);
            #endif
            remove_motif_mutation(nsga2Params, ind, mutation_parameter);
            break;
    }

#endif

    fflush(stdout);

    return;
}


/**
 * Functions related to neurons
 */

/* This mutation changes something on the selected neurons depending on the mutation code */
void neuron_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code){
    // select neurons to change
    int i, j = 0;

    // select neuron to do the changes
    int_array_t *selected_neurons = select_neurons_to_change(nsga2Params, ind);
    
    int mutation_parameter = rnd(0, 3); // the code indicates what mutation will be done


    // loop over dynamic list of neurons to find the selected ones and change them
    neuron_node_t *neuron_node = ind->neurons;
    for(i = 0; i<selected_neurons->n; i++){
        
        // loop until we reach the selected neuron node
        while(j != selected_neurons->array[i]){
            neuron_node = neuron_node->next_neuron;
            j++;
        }

        // change neuron property value depending on mutation type
        switch(mutation_parameter){
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
            case 3: // change resistance
                neuron_node->r = neuron_node->r + (rndreal(-0.25, 0.25));
                break;
        }        
    }

    // deallocate memory
    deallocate_int_arrays(selected_neurons, 1);
}

/* Select randomly what neurons will be mutated */
int_array_t* select_neurons_to_change(NSGA2Type *nsga2Params, individual *ind){
    int i;

    int_array_t *selected_neurons;

    int min, max; // select the amount of neurons to mutate
    min = (int)(ind->n_motifs * nsga2Params->min_neurons);
    max = (int)(ind->n_motifs * nsga2Params->max_neurons);

    if(max > ind->n_neurons)
        max = ind->n_neurons;

    selected_neurons = (int_array_t *)malloc(sizeof(int_array_t));
    selected_neurons->n = rnd(min, max); // for now only one
    selected_neurons->array = malloc(selected_neurons->n * sizeof(int));

    // select neurons to change values
    for(i = 0; i<selected_neurons->n; i++)
        selected_neurons->array[i] = rnd(0, ind->n_neurons - 1);

    // order the list
    insertion_sort(selected_neurons->array, selected_neurons->n);

    // return the list
    return selected_neurons;
}


/**
 * Functions related to synapses
 */

/* This mutation changes something on the selected synapses depending on the mutation code */
void synapse_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code){
    // select neurons to change
    int i, j = 0, s = 0, mutation_parameter;

    // select what property should be changed
    if(nsga2Params->weights_included == 1)
        mutation_parameter = rnd(0, 1); // the code indicates what mutation will be done
    else
        mutation_parameter = 0;

    // select neuron to do the changes
    int_array_t *selected_synapses = select_synapses_to_change(nsga2Params, ind);
    
    // loop over dynamic list of neurons to find the selected ones and change them
    sparse_matrix_node_t *synapse_node = ind->connectivity_matrix;

    s = 0; // index of the synapse inside in the synapse node


    printf(" > > n_synapses = %d, n_input_synapses = %d, selected synapses: ", ind->n_synapses, ind->n_input_synapses);
    for(i = 0; i<selected_synapses->n; i++){
        printf("%d,", selected_synapses->array[i]);
    }
    printf("\n");
    fflush(stdout);

    int counter = 0;
    while(synapse_node){
        counter += abs(synapse_node->value);
        synapse_node = synapse_node->next_element;
    }
    printf(" > > Counter %d\n", counter);
    fflush(stdout);
    synapse_node = ind->connectivity_matrix;

    // loop over synapses to find those that must be mutated
    for(i = 0; i<selected_synapses->n; i++){
        //printf(" > > > i = %d, j = %d, s = %d, selected = %d\n", i, j, s, selected_synapses->array[i]);
        // loop until we reach the selected neuron node
        while(j != selected_synapses->array[i]){

            // each node has several synapses, so loop over those
            if(s == abs(synapse_node->value) - 1){
                s = 0;
                synapse_node = synapse_node->next_element;
            }
            else{
                s++;
            }
            
            j++;
        }

        switch(mutation_parameter){
            case 0:
                synapse_node->latency[s] = synapse_node->latency[s] + (rnd(-2, 2));
                if(synapse_node->latency[s] <= 0)
                    synapse_node->latency[s] = 1;
                break;
            case 1:
                synapse_node->weight[s] = synapse_node->weight[s] + (synapse_node->weight[s] * (rndreal(-0.5, 0.5)));
                break;
            case 2:
                synapse_node->learning_rule[s] = synapse_node->learning_rule[s]; // TODO
                break;
        }
    }

    deallocate_int_arrays(selected_synapses, 1);
}

/* Select randomly what synapses will be mutated */
int_array_t* select_synapses_to_change(NSGA2Type *nsga2Params, individual *ind){
    int i, j; //valid;

    int_array_t *selected_synapses;

    int min, max;
    min = (int)(ind->n_motifs * nsga2Params->min_synapses);
    max = (int)(ind->n_motifs * nsga2Params->max_synapses);
    if(max > ind->n_synapses)
        max = ind->n_synapses;

    selected_synapses = (int_array_t *)malloc(sizeof(int_array_t));
    selected_synapses->n = rnd(min, max); // for now only one
    selected_synapses->array = malloc(selected_synapses->n * sizeof(int));

    // select neurons to change values
    i=0;
    //valid = 1; 
    while(i < selected_synapses->n){
        selected_synapses->array[i] = rnd(0, ind->n_synapses - ind->n_input_synapses - 1);//rnd(0, ind->n_synapses - 1);//rnd(0, ind->n_synapses - ind->n_input_synapses - 1);
        //valid = 1;

        /*for(j = 0; j<i; j++)
            if(selected_synapses->array[i] == selected_synapses->array[j])
                valid = 0;

        if(valid == 1)
            i++;*/
    }

    // order the list
    insertion_sort(selected_synapses->array, selected_synapses->n);

    // return the list
    return selected_synapses;
}



/**
 * Functions related to the mutation of adding motifs
 */

/* This mutation adds a new motif (or more than one) to the network (with random connections) */
/* new_motifs_types, selected_input_motifs and selected_output_motifs can be provided as parameter, but it is better to be NULL as 
   input values are used for testing */
void add_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int *new_motifs_types, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs){
    int i = 0, j, s;

    int last_motif_id; // vairables to store the number of new motifs that are added to the genotype and the id of the last processed motif
    new_motif_t *motif_node; // motif nodes to store the new motifs and to loop over the dynamic list of motifs
    neuron_node_t *neuron_node; // neuron node to loop over the dynamic list of neurons and to add new ones
    int min_connected_motifs, max_connected_motifs;
    int_array_t *all_selected_output_motifs, *all_selected_input_motifs;


    // select the types of the new motifs
    if(!new_motifs_types){ // if motifs are not provided in the input, allocate memory and choose types
        new_motifs_types = (int *)malloc(n_new_motifs * sizeof(int));
        for(i = 0; i<n_new_motifs; i++){
            new_motifs_types[i] = rnd(0, n_motifs-1);
        }
    }

    // Print dynamic lists before mutation
#ifdef DEBUG3
    printf(" =================== \n> > > > > In add motif mutation. Printing input motifs before mutation:\n");
    for(i = 0; i<ind->n_motifs; i++){
        printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
        if(ind->connectivity_info.in_connections[i].n_nodes > 0){
            int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

            while(int_node){
                printf("%d ", int_node->value);
                int_node = int_node->next;
            }
        }
        printf("\n");
    }

    printf(" > > > > > Printing output motifsn before mutation:\n");
    for(i = 0; i<ind->n_motifs; i++){
        printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
        if(ind->connectivity_info.out_connections[i].n_nodes > 0){
            int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

            while(int_node){
                printf("%d ", int_node->value);
                int_node = int_node->next;
            }
        }
        printf("\n");
    }

    printf("\n > > > > Updating dynamic lists...\n");
    fflush(stdout);

#endif

    // add the number of new motifs to the individual
    ind->n_motifs += n_new_motifs;
    
    // add new motif to the lists of motifs and neurons
    add_motifs_and_neurons_to_dynamic_lists(nsga2Params, ind, n_new_motifs, new_motifs_types);


    /* Select input and output motifs for each new motif, and then map all to input connections */
    //min_connected_motifs = 1; // TODO: randomization and network dependency
    //max_connected_motifs = 3; // TODO: randomization and network dependency
    min_connected_motifs = (int)(ind->n_motifs * nsga2Params->min_percentage_connectivity);
    max_connected_motifs = (int)(ind->n_motifs * nsga2Params->max_percentage_connectivity);  
    
    if(!selected_input_motifs || !selected_output_motifs){
        selected_input_motifs = select_motifs(nsga2Params, ind, n_new_motifs, min_connected_motifs ,max_connected_motifs); // select input motifs for each new motif
        selected_output_motifs = select_motifs(nsga2Params, ind, n_new_motifs, min_connected_motifs, max_connected_motifs); // selected output motifs for each new motif
    }

    // map from input and output list received in this function to only a input list TODO: get directly this list instead of mapping, optimization
    all_selected_input_motifs = map_IO_motifs_to_input(ind, n_new_motifs, selected_input_motifs, selected_output_motifs); // map


    #ifdef DEBUG3
        printf(" > > > > Actual motifs: ");
        motif_node = ind->motifs_new;
        while(motif_node){
            printf("%d (%d) ", motif_node->motif_type, motif_node->initial_global_index);
            motif_node = motif_node->next_motif;
        }
        printf("\n");

        printf(" > > > > New motif types: ");
        for(i = 0; i<n_new_motifs; i++){
            printf("%d ", new_motifs_types[i]);
        }
        printf("\n");

        printf(" > > > > Input motifs for each new motif: \n");
        for(i = 0; i<n_new_motifs; i++){
            printf(" > > > > > Motif %d: ", i + ind->n_motifs - n_new_motifs);
            for(j = 0; j<selected_input_motifs[i].n; j++){
                printf("%d ", selected_input_motifs[i].array[j]);
            }
            printf("\n");
        }
        printf(" > > > > Output motifs for each new motif: \n");
        for(i = 0; i<n_new_motifs; i++){
            printf(" > > > > > Motif %d: ", i + ind->n_motifs - n_new_motifs);
            for(j = 0; j<selected_output_motifs[i].n; j++){
                printf("%d ", selected_output_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);
    #endif


    // TODO: This in only a temporal solution to map from the partial list of the inputs and outputs of the new motifs to the entire dynamic lists that will be used in the future
    // this solution is not efficient nor in memory usage neither in performance, so it must be refaztorized as fast as possible

    #ifdef DEBUG3
        printf(" > > > > Mapping from input motifs to output motifs...\n");
        fflush(stdout);
    #endif


    // map from all input to all output
    all_selected_output_motifs = map_from_input_motifs_to_output(ind, ind->n_motifs, n_new_motifs, max_connected_motifs, all_selected_input_motifs);

    #ifdef DEBUG3
        printf(" > > > > Mapped!\n");

        printf(" > > > > Printing all new input motifs: \n");

        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > Motif %d (%d): ", i, all_selected_input_motifs[i].n);
            for(j = 0; j<all_selected_input_motifs[i].n; j++){
                printf("%d ", all_selected_input_motifs[i].array[j]);
            }
            printf("\n");
        }

        printf(" > > > > Printing all new output motifs: \n");

        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > Motif %d (%d): ", i, all_selected_output_motifs[i].n);
            for(j = 0; j<all_selected_output_motifs[i].n; j++){
                printf("%d ", all_selected_output_motifs[i].array[j]);
            }
            printf("\n");
        }
        fflush(stdout);
    #endif


    /* Manage dynamic lists that indicate which motifs are input and output for each motif */

    #ifdef DEBUG3
        printf(" > > > > Adding new motifs to dynamic lists\n");
        fflush(stdout);
    #endif


    // if the array is not large enough to store all the new motifs, realloc // TODO: This should be moved to another place

    if(ind->connectivity_info.n_max_motifs < ind->n_motifs){
        #ifdef DEBUG3
            printf(" > > > > Reallocating memory!!!!\n");
        #endif
        ind->connectivity_info.n_max_motifs = ind->n_motifs * 5;
        ind->connectivity_info.in_connections = realloc(ind->connectivity_info.in_connections, ind->connectivity_info.n_max_motifs); 
        ind->connectivity_info.out_connections = realloc(ind->connectivity_info.out_connections, ind->connectivity_info.n_max_motifs);
    }


    // Update the list of connections for each motif
    add_new_input_motifs_to_connectivity_lists(ind, ind->connectivity_info.in_connections, all_selected_input_motifs);
    add_new_input_motifs_to_connectivity_lists(ind, ind->connectivity_info.out_connections, all_selected_output_motifs);


    /*int_node_t *int_node, *prev, *next, *tmp;
    for(i = 0; i<ind->n_motifs; i++){
        // I think that this could be refactorized to a function to avoid repeating the same code two times
        j = 0;

        // check if i.th motif has new input or output motifs
        if(all_selected_input_motifs[i].n > 0){

            // if motif already has input motif, then only add new ones. Else, initialize the list
            if(ind->connectivity_info.in_connections[i].n_nodes == 0){
                
                int_node = initialize_and_allocate_int_node(all_selected_input_motifs[i].array[0], NULL, NULL); // initialize the int_node
                ind->connectivity_info.in_connections[i].first_node = int_node; // connect the dynamic list to the new first int_node
                ind->connectivity_info.in_connections[i].n_nodes ++; // update the number of nodes
                j++;
            
            }
            // check if the new first motif must be located before the actual first one
            else if (all_selected_input_motifs[i].array[0] < ind->connectivity_info.in_connections[i].first_node->value){

                tmp = ind->connectivity_info.in_connections[i].first_node; // store the first element of the dynamic list
                int_node = initialize_and_allocate_int_node(all_selected_input_motifs[i].array[0], NULL, tmp);
                ind->connectivity_info.in_connections[i].first_node = int_node; // connect the dynamic list with the new first node
                ind->connectivity_info.in_connections[i].n_nodes ++; // update the number of nodes
                j++;

            }

            // get the first element, the previous one and the next one
            int_node = ind->connectivity_info.in_connections[i].first_node;
            prev = int_node->prev; // must be NULL as we are in the first element
            next = int_node->next;


            // add the rest motifs to the dynamic list
            for(j; j<all_selected_input_motifs[i].n; j++){
           
                // get the next and the previous nodes
                next = int_node->next;
                prev = int_node->prev;

                int val = all_selected_input_motifs[i].array[j]; // obtain the value (motif) that will be added to the list

                while(int_node->next && int_node->next->value < val){
                    int_node = int_node->next;
                    next = int_node->next;
                    prev = int_node->prev;
                }

                // if there is no next motif, so add directly in the final of the list
                if(next == NULL){
                    prev = int_node;
                    next = NULL;
                    int_node = initialize_and_allocate_int_node(all_selected_input_motifs[i].array[j], prev, next);
                    ind->connectivity_info.in_connections[i].n_nodes ++; // update the number of nodes
                }
                // in case it is not the last, find where the new node must be located and add to the list
                else{
                    if(val >= int_node->value && val <int_node->next->value){
                        prev = int_node;
                        next = int_node->next;
                        int_node = initialize_and_allocate_int_node(all_selected_input_motifs[i].array[j], prev, next);

                        // a new motif has been added
                        ind->connectivity_info.in_connections[i].n_nodes ++;
                    }
                }
            }
        }

        j=0;
        if(all_selected_output_motifs[i].n > 0){

            // if motif already has input motif, then only add new ones. Else, initialize the list
            if(ind->connectivity_info.out_connections[i].n_nodes == 0){
                
                int_node = initialize_and_allocate_int_node(all_selected_output_motifs[i].array[0], NULL, NULL); // initialize the int_node
                ind->connectivity_info.out_connections[i].first_node = int_node; // connect the dynamic list to the new first int_node
                ind->connectivity_info.out_connections[i].n_nodes ++; // update the number of nodes
                j++;

            }
            // check if the new first motif must be located before the actual first one
            else if (all_selected_output_motifs[i].array[0] < ind->connectivity_info.out_connections[i].first_node->value){

                tmp = ind->connectivity_info.out_connections[i].first_node; // store the first element of the dynamic list
                int_node = initialize_and_allocate_int_node(all_selected_output_motifs[i].array[0], NULL, tmp);
                ind->connectivity_info.out_connections[i].first_node = int_node; // connect the dynamic list with the new first node
                ind->connectivity_info.out_connections[i].n_nodes ++; // update the number of nodes
                j++;

            }

            // get the first element, the previous one and the next one
            int_node = ind->connectivity_info.out_connections[i].first_node;
            prev = int_node->prev; // must be NULL as we are in the first element
            next = int_node->next;


            // add the rest motifs to the dynamic list
            for(j; j<all_selected_output_motifs[i].n; j++){
           
                // get the next and the previous nodes
                next = int_node->next;
                prev = int_node->prev;

                int val = all_selected_output_motifs[i].array[j]; // obtain the value (motif) that will be added to the list

                while(int_node->next && int_node->next->value < val){
                    int_node = int_node->next;
                    next = int_node->next;
                    prev = int_node->prev;
                }

                // if there is no next motif, so add directly in the final of the list
                if(next == NULL){
                    prev = int_node;
                    next = NULL;
                    int_node = initialize_and_allocate_int_node(all_selected_output_motifs[i].array[j], prev, next);
                    ind->connectivity_info.out_connections[i].n_nodes ++; // update the number of nodes
                }
                // in case it is not the last, find where the new node must be located and add to the list
                else{
                    if(val >= int_node->value && val <int_node->next->value){
                        prev = int_node;
                        next = int_node->next;
                        int_node = initialize_and_allocate_int_node(all_selected_output_motifs[i].array[j], prev, next);

                        // a new motif has been added
                        ind->connectivity_info.out_connections[i].n_nodes ++;
                    }
                }
            }
        }    
    }*/

    #ifdef DEBUG3
        printf(" > > > > > Printing input motifs:\n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
            if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }
        
        printf(" > > > > > Printing output motifs:\n");
        for(i = 0; i<ind->n_motifs; i++){
            printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
            if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                while(int_node){
                    printf("%d ", int_node->value);
                    int_node = int_node->next;
                }
            }
            printf("\n");
        }

        printf("\n > > > > Updating dynamic lists...\n");
        fflush(stdout);

    #endif


    #ifdef DEBUG3
        printf(" > > > > Added!\n");
        fflush(stdout);
    #endif

    // free memory that will not be used
    free(new_motifs_types);
    deallocate_int_arrays(selected_input_motifs, n_new_motifs);
    deallocate_int_arrays(selected_output_motifs, n_new_motifs);
    deallocate_int_arrays(all_selected_output_motifs, ind->n_motifs);


    /* Add the new synaptic connectiosn to the connections list */
    #ifdef DEBUG3
    printf(" > > > > Updating sparse matrix...\n");
    #endif

    #ifdef DEBUG3
            int *rrmatrix = (int *)calloc((ind->n_neurons - n_new_motifs) * (ind->n_neurons - n_new_motifs), sizeof(int));
            get_complete_matrix_from_dynamic_list(rrmatrix, ind->connectivity_matrix, ind->n_neurons);

            print_synapses_dynamic_list(ind->connectivity_matrix);
            print_connectivity_matrix(rrmatrix, ind->n_neurons - n_new_motifs);

            free(rrmatrix);

            fflush(stdout);
    #endif

    // update sparse matrix
    update_sparse_matrix_add_motifs(nsga2Params, ind, n_new_motifs, all_selected_input_motifs);


    #ifdef DEBUG3
    printf(" > > > > Sparse matrix updated!\n");
    #endif

    #ifdef DEBUG3
            int *rmatrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
            get_complete_matrix_from_dynamic_list(rmatrix, ind->connectivity_matrix, ind->n_neurons);

            print_synapses_dynamic_list(ind->connectivity_matrix);
            print_connectivity_matrix(rmatrix, ind->n_neurons);

            free(rmatrix);
        printf(" MUTATION FINISHED!!\n");
            fflush(stdout);
    #endif

    // reinitialize motifs neuron identifiers TODO: I would say this is not neccessary when adding new motifs
    /*motif_node = ind->motifs_new;
    motif_t *motif_data;
    int global_index = 0;
    while(motif_node){
        motif_node->initial_global_index = global_index;
        motif_data = &(motifs_data[motif_node->motif_type]);
        global_index += motif_data->n_neurons;
        motif_node = motif_node->next_motif;
    }*/


    deallocate_int_arrays(all_selected_input_motifs, ind->n_motifs);
}

/* Function to update the dynamic lists of motifs and neurons (add new motifs and neurons) */
void add_motifs_and_neurons_to_dynamic_lists(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int *new_motifs_types){
    int i, j, last_motif_id, behav;
    new_motif_t *motif_node;
    neuron_node_t *neuron_node;

    // get the first motif and neuron
    motif_node = ind->motifs_new; // first motif
    neuron_node = ind->neurons;

    // loop until the last motif is adquired // TODO: Optimization: add a pointer to the last element of the list
    while(motif_node->next_motif)
        motif_node = motif_node->next_motif;

    // store last motif id, as the new motif id will be the following one (id + 1)
    last_motif_id = motif_node->motif_id;


    // get the last neuron in the list // TODO: Optimization: add a pointer to the last element of the list
    while(neuron_node->next_neuron)
        neuron_node = neuron_node->next_neuron;


    // add new motifs, and for each motif, its neurons, to the dynamic lists
    for(i = 0; i<n_new_motifs; i++){
        // update motif id for the next one
        last_motif_id ++;

        // allocate memory for the next motif (the new one)
        motif_node = initialize_and_allocate_motif_from_type(motif_node, last_motif_id, ind, new_motifs_types[i]);

        //new_motifs_types[i] = motif_node->motif_type;

        // add motif neurons to the dynamic list of neurons
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++){
            behav = motifs_data[motif_node->motif_type].neuron_behaviour[j];
            neuron_node = initialize_and_allocate_neuron_node_and_behaviour(nsga2Params, neuron_node, behav); // temporal function that includes neuron behaviour in the initialization
        }
    }
}

/* Function to add new motifs to the dynamic lists that indicate to which motifs each motif is connected */
void add_new_input_motifs_to_connectivity_lists(individual *ind, int_dynamic_list_t *connections, int_array_t *selected_motifs){
    
    int i, j;
    int_node_t *int_node, *prev, *next, *tmp;
    
    for(i = 0; i<ind->n_motifs; i++){
        // I think that this could be refactorized to a function to avoid repeating the same code two times
        j = 0;

        // check if i.th motif has new input or output motifs
        if(selected_motifs[i].n > 0){

            // if motif already has input motif, then only add new ones. Else, initialize the list
            if(connections[i].n_nodes == 0){
                
                int_node = initialize_and_allocate_int_node(selected_motifs[i].array[0], NULL, NULL); // initialize the int_node
                connections[i].first_node = int_node; // connect the dynamic list to the new first int_node
                connections[i].n_nodes ++; // update the number of nodes
                j++;
            
            }
            // check if the new first motif must be located before the actual first one
            else if (selected_motifs[i].array[0] < connections[i].first_node->value){

                tmp = connections[i].first_node; // store the first element of the dynamic list
                int_node = initialize_and_allocate_int_node(selected_motifs[i].array[0], NULL, tmp);
                connections[i].first_node = int_node; // connect the dynamic list with the new first node
                connections[i].n_nodes ++; // update the number of nodes
                j++;

            }

            // get the first element, the previous one and the next one
            int_node = connections[i].first_node;
            prev = int_node->prev; // must be NULL as we are in the first element
            next = int_node->next;


            // add the rest motifs to the dynamic list
            for(j; j<selected_motifs[i].n; j++){
           
                // get the next and the previous nodes
                next = int_node->next;
                prev = int_node->prev;

                int val = selected_motifs[i].array[j]; // obtain the value (motif) that will be added to the list

                while(int_node->next && int_node->next->value < val){
                    int_node = int_node->next;
                    next = int_node->next;
                    prev = int_node->prev;
                }

                // if there is no next motif, so add directly in the final of the list
                if(next == NULL){
                    prev = int_node;
                    next = NULL;
                    int_node = initialize_and_allocate_int_node(selected_motifs[i].array[j], prev, next);
                    connections[i].n_nodes ++; // update the number of nodes
                }
                // in case it is not the last, find where the new node must be located and add to the list
                else{
                    if(val >= int_node->value && val <int_node->next->value){
                        prev = int_node;
                        next = int_node->next;
                        int_node = initialize_and_allocate_int_node(selected_motifs[i].array[j], prev, next);

                        // a new motif has been added
                        connections[i].n_nodes ++;
                    }
                }
            }
        }    
    }
}

/* Function to select input or output motifs for new motifs */
int_array_t* select_motifs(NSGA2Type *nsga2Params, individual *ind, int n_motifs, int min_connected_motifs, int max_connected_motifs){
    int i, j;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(n_motifs * sizeof(int_array_t)); // selected input/output motifs per each new motif
    
    // per each new motif, select input or output motifs
    for(i = 0; i<n_motifs; i++){
        // select amount of motifs and allocate memory
        selected_motifs[i].n = rnd(min_connected_motifs, max_connected_motifs); // TODO: this should not be 1 always
        selected_motifs[i].array = malloc(selected_motifs[i].n * sizeof(int));

        for(j = 0; j<selected_motifs[i].n; j++){
            selected_motifs[i].array[j] = rnd(0, ind->n_motifs - 1); // the new motif can be connected with itself
        }
        // order the list
        insertion_sort(selected_motifs[i].array, selected_motifs[i].n);
    }

    // return the list
    return selected_motifs;
}

/* Select randomly the input and output motifs for the new motifs TODO: Not implemented yet */
int_array_t* select_motifs_to_connect_with_new_motifs(individual *ind, int n_motifs, int n_new_motifs, int min_connected_motifs, int max_connected_motifs){
    int i, j;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(n_motifs * sizeof(int_array_t)); // selected input/output motifs per each new motif
    

    // per each new motif, select input or output motifs
    for(i = 0; i<n_motifs; i++){
        // select amount of motifs and allocate memory
        selected_motifs[i].n = rnd(min_connected_motifs, max_connected_motifs); // TODO: this should not be 1 always
        selected_motifs[i].array = malloc(selected_motifs[i].n * sizeof(int));

        for(j = 0; j<selected_motifs[i].n; j++){
            selected_motifs[i].array[j] = rnd(0, ind->n_motifs - 1); // the new motif can be connected with itself
        }

        // order the list
        insertion_sort(selected_motifs[i].array, selected_motifs[i].n);
    }

    // return the list
    return selected_motifs;
}


/* Update the sparse matrix adding the new connections */
void update_sparse_matrix_add_motifs(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int_array_t *selected_input_motifs){
    int i, j;

    new_motif_t *motif_node, *tmp_motif_node;
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

        //printf(" motif type: %d, first neuron: %d\n", SMBI.motifs_types[i], SMBI.motifs_first_neuron_indexes[i]);

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
                    while(synapse_node->next_element != NULL && synapse_node->col == synapse_node->next_element->col){
                        synapse_node = synapse_node->next_element;
                    }

                    // store next column first element to make the connection later
                    next_column_synapse_node = synapse_node->next_element; 

                    // add new cells to complete the column if the neuron is an output neuron
                    if(check_if_neuron_is_input(SMBI.actual_motif_type, j) == 1){
                        synapse_node = build_neuron_sparse_matrix_column(nsga2Params, ind, synapse_node, &(selected_input_motifs[i]), &SMBI);
                    }
                    
                    // rconnect the last element newly added with the next column first element
                    synapse_node->next_element = next_column_synapse_node;
                }

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
        synapse_node = build_motif_sparse_matrix_columns(nsga2Params, ind, synapse_node, &(selected_input_motifs[i]), &SMBI);

        // move to the next motif
        motif_node->next_motif;
    }

    // set NULL after the last synapse of the sparse matrix
    synapse_node->next_element = NULL;

    // print the matrix
    #ifdef DEBUG3
        // print connectivity matrix
        int *result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
        get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

        print_synapses_dynamic_list(ind->connectivity_matrix);
        print_connectivity_matrix(result_matrix, ind->n_neurons);

        free(result_matrix);

        fflush(stdout);
    #endif
    
    // free allocated memory
    free(SMBI.motifs_types);
    free(SMBI.motifs_first_neuron_indexes);
}


/**
 * Functions for removing motifs of an individual
 */

/* Function to selected which motifs will be removed and call the function to remove */
void remove_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_remove_motifs){

    // select motifs to remove
    int_array_t *selected_motifs_to_remove = select_motifs_to_be_removed(ind, n_remove_motifs);
    remove_selected_motifs_mutation(nsga2Params, ind, selected_motifs_to_remove);
}

/* Function to remove motifs of an individual */
void remove_selected_motifs_mutation(NSGA2Type *nsga2Params, individual *ind, int_array_t *selected_motifs_to_remove){
    int i, j, s, remove_index;
    new_motif_t *previous_motif_node, *tmp_motif_node, *motif_node;
    neuron_node_t *previous_neuron_node, *neuron_node, *tmp_neuron_node;
    motif_t *motif_data;
    int_array_t *tmp_selected_motifs_to_remove, *new_selected_motifs_to_remove;
    int rest_neurons = 0, acc = 0;

    int n_remove_motifs = selected_motifs_to_remove->n;

    // update sparse matrix removing neccessary synapses

#ifdef DEBUG3
    printf(" > > > > In remove motif mutation. Printing individiual\n");
    
    printf(" > > > > > n motifs = %d: ", ind->n_motifs);
    tmp_motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d ", tmp_motif_node->motif_type);
    }
    printf("\n");

    printf(" > > > > > Printing connectivity:\n");

            printf(" > > > > > Printing input motifs:\n");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > > Printing output motifs:");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > Dynamic list updated!\n");
            fflush(stdout);


    printf(" > > > > > Selected motifs to be removed: ");
    for(i = 0; i<selected_motifs_to_remove->n; i++){
        printf("%d ", selected_motifs_to_remove->array[i]);
    }
    printf("\n");
    fflush(stdout);


#endif


    while(n_remove_motifs > 0){

        #ifdef DEBUG3
            printf(" > > > > Printing selected motifs to be removed (total %d): ", ind->n_motifs);

            for(i = 0; i<selected_motifs_to_remove->n; i++)
                printf("%d ", selected_motifs_to_remove->array[i]);
            printf("\n");
            printf(" > > > > Updating sparse matrix...\n");
            fflush(stdout);
        #endif

        // ==================== //
        // update sparse matrix //
        // ==================== //

        #ifdef DEBUG3
        printf(" Printing sparse matrix, first before mutation, and after mutation then\n");
            int *rmatrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
            get_complete_matrix_from_dynamic_list(rmatrix, ind->connectivity_matrix, ind->n_neurons);
            print_synapses_dynamic_list(ind->connectivity_matrix);
            print_connectivity_matrix(rmatrix, ind->n_neurons);
            free(rmatrix);
            fflush(stdout);
        #endif

        update_sparse_matrix_remove_motifs(ind, selected_motifs_to_remove);
        #ifdef DEBUG3
            rmatrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
            get_complete_matrix_from_dynamic_list(rmatrix, ind->connectivity_matrix, ind->n_neurons);
            print_synapses_dynamic_list(ind->connectivity_matrix);
            print_connectivity_matrix(rmatrix, ind->n_neurons);
            free(rmatrix);
            fflush(stdout);
        #endif
        #ifdef DEBUG3
            printf(" > > > > Matrix updated\n");
            printf(" > > > > Updating dynamic lists...\n");
            fflush(stdout);    
        #endif




        // ==================== //
        // update dynamic lists //
        // ==================== //
        
        #ifdef DEBUG3
            printf(" > > > > > Printing input motifs:\n");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > > Printing output motifs:");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf("\n > > > > Updating dynamic lists...\n");
            fflush(stdout);

        #endif


        new_selected_motifs_to_remove = remove_motifs_from_dynamic_list(ind, selected_motifs_to_remove);

        // reorder array as now some motifs have been removed // Remove the selected motifs?
        i = 0; // index of the motif that must be processed
        j = 0; // counter of how much motifs have been processed
        int s; // helper index       
        int_node_t *int_node, *tmp_node;
        int tmp_n_motifs = ind->n_motifs;
        // loop until all selected motifs are removed
        while(j < n_remove_motifs){
            if(i + j == selected_motifs_to_remove->array[j]){

                // free the dynamic lists of this motif
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node = ind->connectivity_info.in_connections[i].first_node;
                    tmp_node;
                    while(int_node){
                        tmp_node = int_node;
                        int_node = int_node->next;
                        free(tmp_node);
                    }
                    ind->connectivity_info.in_connections[i].first_node = NULL;
                    ind->connectivity_info.in_connections[i].n_nodes = 0;
                }
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node = ind->connectivity_info.out_connections[i].first_node;
                    tmp_node;
                    while(int_node){
                        tmp_node = int_node;
                        int_node = int_node->next;
                        free(tmp_node);
                    }
                    ind->connectivity_info.out_connections[i].first_node = NULL;
                    ind->connectivity_info.out_connections[i].n_nodes = 0;
                }

                // now move the rest motifs 
                for(s = i; s<tmp_n_motifs-1; s++){
                    ind->connectivity_info.in_connections[s].first_node = ind->connectivity_info.in_connections[s+1].first_node;
                    ind->connectivity_info.in_connections[s].n_nodes = ind->connectivity_info.in_connections[s+1].n_nodes;
                    ind->connectivity_info.out_connections[s].first_node = ind->connectivity_info.out_connections[s+1].first_node;
                    ind->connectivity_info.out_connections[s].n_nodes = ind->connectivity_info.out_connections[s+1].n_nodes;
                }
                i--;
                j++;
                tmp_n_motifs --;
            }
            // move to the next motif
            i++;
        }

        /*for(i = 0; i<ind->n_motifs; i++){
            printf(" Processing motif %d\n", i);
            fflush(stdout);
            if(j < n_remove_motifs && i == selected_motifs_to_remove->array[j]){
                
                // free the dynamic lists of this motif
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;
                    int_node_t *tmp_node;
                    while(int_node){
                        tmp_node = int_node;
                        int_node = int_node->next;
                        free(tmp_node);
                    }
                    ind->connectivity_info.in_connections[i].first_node = NULL;
                    ind->connectivity_info.in_connections[i].n_nodes = 0;
                }
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;
                    int_node_t *tmp_node;
                    while(int_node){
                        tmp_node = int_node;
                        int_node = int_node->next;
                        free(tmp_node);
                    }
                    ind->connectivity_info.out_connections[i].first_node = NULL;
                    ind->connectivity_info.out_connections[i].n_nodes = 0;
                }

                printf("Reordering...\n");
                fflush(stdout);
                // now move the rest motifs 
                for(s = i; s<ind->n_motifs-1; s++){
                    ind->connectivity_info.in_connections[s].first_node = ind->connectivity_info.in_connections[s+1].first_node;
                    ind->connectivity_info.in_connections[s].n_nodes = ind->connectivity_info.in_connections[s+1].n_nodes;
                    ind->connectivity_info.out_connections[s].first_node = ind->connectivity_info.out_connections[s+1].first_node;
                    ind->connectivity_info.out_connections[s].n_nodes = ind->connectivity_info.out_connections[s+1].n_nodes;
                }
                //i--;
                j++;
            }
        }*/
        fflush(stdout);
        for(i = ind->n_motifs - n_remove_motifs; i<ind->n_motifs; i++){
            ind->connectivity_info.in_connections[i].first_node = NULL;
            ind->connectivity_info.in_connections[i].n_nodes = 0;
            ind->connectivity_info.out_connections[i].first_node = NULL;
            ind->connectivity_info.out_connections[i].n_nodes = 0;
        }


        //
        int acc;
        for(i = 0; i<ind->n_motifs - n_remove_motifs; i++){

            // input
            acc = 0;
            remove_index = 0;
            int_node = ind->connectivity_info.in_connections[i].first_node;
            
            while(int_node){
                if(remove_index < selected_motifs_to_remove->n){
                    if(int_node->value < selected_motifs_to_remove->array[remove_index]){
                        int_node->value -= acc;
                        int_node = int_node->next;
                    }
                    else{ // >
                        acc++;
                        remove_index ++;
                    }
                }
                else{
                    int_node->value -= acc;
                    int_node = int_node->next;
                }
            }

            // output
            acc = 0;
            remove_index = 0;
            int_node = ind->connectivity_info.out_connections[i].first_node;
            
            while(int_node){
                if(remove_index < selected_motifs_to_remove->n){
                    if(int_node->value < selected_motifs_to_remove->array[remove_index]){
                        int_node->value -= acc;
                        int_node = int_node->next;
                    }
                    else{ // >
                        acc++;
                        remove_index ++;
                    }
                }
                else{
                    int_node->value -= acc;
                    int_node = int_node->next;
                }
            }
        }



        ind->n_motifs -= n_remove_motifs; // update amount of motifs in the individual

        // remove deleted motifs, and for each motif, its neurons, from the dynamic lists
        motif_node = ind->motifs_new;
        neuron_node = ind->neurons;



        #ifdef DEBUG3
            printf(" > > > > > Printing input motifs:\n");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > > Printing output motifs:");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > Dynamic list updated!\n");
            fflush(stdout);

        #endif



        // remove motifs
        i = 0;
        j = 0; 
        
        // loop until selected motifs are removed from the list

        #ifdef DEBUG3
            printf(" > > > > Updating lists of motifs and neurons..\n");
            fflush(stdout);
        #endif
        
        j=0;
        i = 0;
        
        while(j < selected_motifs_to_remove->n){

            tmp_motif_node = motif_node;
            tmp_motif_node->initial_global_index -= rest_neurons;
            motif_data = &(motifs_data[tmp_motif_node->motif_type]);

            /*if(tmp_motif_node->initial_global_index < 0){
                printf(" HERE NEGATIVE!!!\n");
            }*/

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

        // reinitialize motifs neuron identifiers
        motif_node = ind->motifs_new;
        int global_index = 0;
        while(motif_node){
            motif_node->initial_global_index = global_index;
            motif_data = &(motifs_data[motif_node->motif_type]);
            global_index += motif_data->n_neurons;
            motif_node = motif_node->next_motif;
        }


        #ifdef DEBUG3
            printf(" > > > > Lists of motifs and neurons updated!\n");
            fflush(stdout);
        #endif


        #ifdef DEBUG3
            printf(" > > > > Mapping indexes from old indexes to new ones (some motifs have been removed)...\n");
            fflush(stdout);

            printf(" > > > > > New selected (n = %d): ", new_selected_motifs_to_remove->n);
            for(i = 0; i<new_selected_motifs_to_remove->n; i++)
                printf("%d ", new_selected_motifs_to_remove->array[i]);
            printf("\n");

            printf("> > > > > Old (n = %d): ", selected_motifs_to_remove->n);
            for(i = 0; i<selected_motifs_to_remove->n; i++)
                printf("%d ", selected_motifs_to_remove->array[i]);
            printf("\n");

            fflush(stdout);
        #endif


        // update indexes of motifs that must be removed in the next iteration
        for(i = 0; i<new_selected_motifs_to_remove->n; i++){

            j = 0;
            acc = 0;

            while(j < selected_motifs_to_remove->n && selected_motifs_to_remove->array[j] < new_selected_motifs_to_remove->array[i]){
                acc++;
                j++;
            }
        
            new_selected_motifs_to_remove->array[i] -= acc;

        }

        #ifdef DEBUG3
            printf(" > > > > Mapped! Printing new indexes: ");
            for(i = 0; i<new_selected_motifs_to_remove->n; i++)
                printf("%d ", new_selected_motifs_to_remove->array[i]);
            printf("\n");
            fflush(stdout);
        #endif


        //tmp_selected_motifs_to_remove = selected_motifs_to_remove;
        deallocate_int_arrays(selected_motifs_to_remove, 1);
        selected_motifs_to_remove = new_selected_motifs_to_remove;

        //deallocate_int_array(tmp_selected_motifs_to_remove);
        n_remove_motifs = selected_motifs_to_remove->n;

        new_selected_motifs_to_remove = NULL;

        // print connectivity matrix
        
        #ifdef DEBUG3
            /*int *result_matrix = (int *)calloc(ind->n_neurons * ind->n_neurons, sizeof(int));
            get_complete_matrix_from_dynamic_list(result_matrix, ind->connectivity_matrix, ind->n_neurons);

            //print_synapses_dynamic_list(ind->connectivity_matrix);
            print_connectivity_matrix(result_matrix, ind->n_neurons);

            free(result_matrix);

            fflush(stdout);*/
        #endif
    }


#ifdef DEBUG3
    printf(" > > > > > n motifs = %d: ", ind->n_motifs);
    tmp_motif_node = ind->motifs_new;
    for(i = 0; i<ind->n_motifs; i++){
        printf("%d ", tmp_motif_node->motif_type);
    }
    printf("\n");

    printf(" > > > > > Printing connectivity:\n");

            printf(" > > > > > Printing input motifs:\n");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.in_connections[i].n_nodes);
                if(ind->connectivity_info.in_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.in_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > > Printing output motifs:");
            for(i = 0; i<ind->n_motifs; i++){
                printf(" > > > > > > Motif %d (%d): ", i, ind->connectivity_info.out_connections[i].n_nodes);
                if(ind->connectivity_info.out_connections[i].n_nodes > 0){
                    int_node_t *int_node = ind->connectivity_info.out_connections[i].first_node;

                    while(int_node){
                        printf("%d ", int_node->value);
                        int_node = int_node->next;
                    }
                }
                printf("\n");
            }

            printf(" > > > > Dynamic list updated!\n");
            fflush(stdout);
    #endif

    deallocate_int_arrays(selected_motifs_to_remove, 1);
}

int_array_t* remove_motifs_from_dynamic_list(individual *ind, int_array_t *selected_motifs_to_remove){
        
    // loop over dynamic lists of motifs input and output motifs to update those
    int i, j;
    
    int_array_t* new_selected_motifs_to_remove = (int_array_t *)malloc(sizeof(int_array_t));
    new_selected_motifs_to_remove->n = 0;
    new_selected_motifs_to_remove->array = (int *)calloc(ind->n_motifs, sizeof(int));  // here n_motifs already is the new amount, not the original one

    int_node_t *int_node, *prev, *next, *tmp;
    
    // TODO: This could be optimized looping only those motifs that will be removed and using the motifs referenced on them?????
    // loop over all motifs to check if they are connected to motifs that will be removed
    for(i = 0; i<ind->n_motifs; i++){
        // first in conenction list, and then out connection list
        j = 0;

        if(ind->connectivity_info.in_connections[i].n_nodes > 0){
        
            // get the first node
            int_node = ind->connectivity_info.in_connections[i].first_node;
            prev = int_node->prev; // NULL
            next = int_node->next;

            // loop over the list of int_nodes, and remove those that it is neccessary
            while(int_node && j < selected_motifs_to_remove->n){ // if int_node is NULL, it means that the entire list have been looped. If j == n, all elements have been removed

                // if the node value is smaller than the value we are looking for, move to the next node
                if(int_node->value < selected_motifs_to_remove->array[j]){
                    // update the int node value, as now there are less motifs
                    //int_node->value -= j;

                    // move to the next int node and store previous one and next one
                    int_node = int_node->next;
                    if(int_node){
                        next = int_node->next;
                        prev = int_node->prev;
                    }
                }

                // if the value is higher, update j value
                else if(int_node->value > selected_motifs_to_remove->array[j]){
                    j++;
                }

                // the values are eqauls, so remove the node (and check for repetitions)
                else{ // ==
                    
                    // connect this with the previous element to the one that will be removed
                    if(int_node->next){
                        int_node->next->prev = int_node->prev;
                    }

                    if(int_node->prev){
                        int_node->prev->next = int_node->next;
                    }

                    // if the node is the first one, then the new first will be the next
                    if(int_node == ind->connectivity_info.in_connections[i].first_node)
                        ind->connectivity_info.in_connections[i].first_node = int_node->next;

                    // deallocate memory
                    tmp = int_node; // store the motif that will be deallocated
                    int_node = int_node->next; // move to the next

                    // check if the tmp motif appears more times in the dynamic list
                    free(tmp); // free memory

                    // update amount of nodes
                    ind->connectivity_info.in_connections[i].n_nodes --;
                }
            }
        }

        // first in conenction list, and then out connection list
        j = 0;

        if(ind->connectivity_info.out_connections[i].n_nodes > 0){
        
            // get the first node
            int_node = ind->connectivity_info.out_connections[i].first_node;
            prev = int_node->prev;
            next = int_node->next;

            // loop over the list of int_nodes, and remove those that it is neccessary
            while(int_node && j < selected_motifs_to_remove->n){

                // if the node value is smaller than the value we are looking for, move to the next node
                if(int_node->value < selected_motifs_to_remove->array[j]){
                    // update the int node value, as now there are less motifs
                    //int_node->value -= j;

                    int_node = int_node->next;
                    if(int_node){
                        next = int_node->next;
                        prev = int_node->prev;
                    }
                }

                // if the value is higher, update j value
                else if(int_node->value > selected_motifs_to_remove->array[j]){
                    j++;
                }

                // the values are eqauls, so remove the node (and check for repetitions)
                else{ // ==
                    
                    // connect this with the previous element to the one that will be removed
                    if(int_node->next)
                        int_node->next->prev = int_node->prev;
                    if(int_node->prev){
                        int_node->prev->next = int_node->next;
                    }

                    // if the node is the first one, then the new first will be the next
                    if(int_node == ind->connectivity_info.out_connections[i].first_node)
                        ind->connectivity_info.out_connections[i].first_node = int_node->next;

                    // deallocate memory
                    tmp = int_node; // store the motif that will be deallocated
                    int_node = int_node->next; // move to the next

                    // check if the tmp motif appears more times in the dynamic list
                    free(tmp); // free memory

                    // update amount of nodes
                    ind->connectivity_info.out_connections[i].n_nodes --;
                }
            }
        }

        // check if this motif must be removed (not input motifs or not output motifs and not included in the list of motifs to be removed)
        if(ind->connectivity_info.out_connections[i].n_nodes == 0 || ind->connectivity_info.in_connections[i].n_nodes == 0){
            int included = 0;

            for(j = 0; j<selected_motifs_to_remove->n; j++){
                if(selected_motifs_to_remove->array[j] == i)
                    included = 1;
            }

            if(included == 0){
                new_selected_motifs_to_remove->array[new_selected_motifs_to_remove->n] = i;
                new_selected_motifs_to_remove->n ++;
            }
        }
    }

    // return int_array that contains the new motifs to be removed
    return new_selected_motifs_to_remove;
}


/* Select randomly which motifs will be removed */
int_array_t* select_motifs_to_be_removed(individual *ind, int n_motifs){
    int i, j, selected = 0, eq = 1, min, max;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(sizeof(int_array_t)); // selected input/output motifs per each new motif
    
    // select motifs to be removed. A motif can not be appear two times
    selected_motifs->n = n_motifs; // TODO: this should not be 1 always
    selected_motifs->array = malloc(selected_motifs->n * sizeof(int));

    while(selected < selected_motifs->n){
        selected_motifs->array[selected] = rnd(0, ind->n_motifs - 1);

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

/* Update sparse matrix removing the selected motifs */
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
    while(n_first_motifs<selected_motifs->n && selected_motifs->array[n_first_motifs] == n_first_motifs){
        n_first_motifs ++;
    }

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

    // free memory
    free(SMBI.motifs_types);
    free(SMBI.motifs_first_neuron_indexes);

    fflush(stdout);
}

/* Remove all synapses for this motif in the dynamic list of synapses, as this motif has been selected to be removed */
sparse_matrix_node_t* remove_all_motif_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, sparse_matrix_node_t *synapse_node){
    
    sparse_matrix_node_t *tmp_synapse_node, *last_synapse_node;

    // store actual motif information
    int actual_motif_index = SMBI->actual_motif_index;
    int actual_motif_type = SMBI->actual_motif_type;
    int actual_motif_first_neuron_index = SMBI->actual_motif_first_neuron_index;
    motif_t *actual_motif_info = SMBI->actual_motif_info;

    // if the actual motif is the last one, remove until synapse node is NULL
    if(actual_motif_index == ind->n_motifs - 1){

        while(synapse_node){
            tmp_synapse_node = synapse_node;
            synapse_node = synapse_node->next_element;

            if(tmp_synapse_node == ind->connectivity_matrix)
                ind->connectivity_matrix = synapse_node;

            // free memory and update number of synapses
            ind->n_synapses -= abs(tmp_synapse_node->value);
            deallocate_sparse_matrix_node(tmp_synapse_node);
        }

        SMBI->previous_synapse_node->next_element = NULL;
    }

    // else, loop until the next motif first synapse is reached
    else{
        while(synapse_node && synapse_node->col < SMBI->motifs_first_neuron_indexes[actual_motif_index+1]){
            tmp_synapse_node = synapse_node;
            synapse_node = synapse_node->next_element;

            if(tmp_synapse_node == ind->connectivity_matrix)
                ind->connectivity_matrix = synapse_node;

            // free memory and update number of synapses
            ind->n_synapses -= abs(tmp_synapse_node->value);
            deallocate_sparse_matrix_node(tmp_synapse_node);
        }

        // once we are in the first column of the new motif, make the connection again between the previous motif and the actual one
        if(SMBI->previous_synapse_node)
            SMBI->previous_synapse_node->next_element = synapse_node;
    }

    return synapse_node;
}

/* Remove selected motifs from the synapses of the actual motif */
sparse_matrix_node_t* remove_selected_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, int_array_t *selected_motifs, sparse_matrix_node_t *synapse_node){
    int i, j, motif_index, motif_type, tmp_motif_index, tmp_motif_first_neuron_index, tmp_motif_type, next_tmp_motif_index = 0;
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
        
        rest_neuron_row = 0;

        // check that we are in the column of the actual neuron (first neuron index + local index)
        if(synapse_node && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){

            // loop over the selected motifs to be removed
            for(j = 0; j<selected_motifs->n; j++){

                motif_index = selected_motifs->array[j];
                motif_type = SMBI->motifs_types[motif_index];
                remove_motif_info = &(motifs_data[motif_type]);

                // the cell is previous to the motif that must be removed, so move to the next cell
                if(synapse_node && synapse_node->row < SMBI->motifs_first_neuron_indexes[motif_index]){
                    // check what motif is being proccessed to update the has_output value for the motifs
                    //tmp_motif_index = 


                    // actual motif has an input motif and the selected one an output motif

                    // loop until we reach to the selected motif or we surpass it
                    //printf(" motif_index %d (%d), synapse_node->row %d, synapse_node->col %d, actual_motif_index %d, i %d\n", motif_index, ind->n_motifs, synapse_node->row, synapse_node->col, actual_motif_index, i);
                    //printf(" first neuron index %d\n", SMBI->motifs_first_neuron_indexes[motif_index]);
                    //fflush(stdout);
                    while(synapse_node && synapse_node->row < SMBI->motifs_first_neuron_indexes[motif_index] && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                        synapse_node->col -= rest_neuron_col;
                        synapse_node->row -= rest_neuron_row;
                        //if(synapse_node->row < 0){ printf(" NEGATIVE!!!\n");}

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
                        deallocate_sparse_matrix_node(tmp_synapse_node);
                    }

                    // once we are in the first column of the new motif, make the connection again between the previous motif and the actual one
                    if(SMBI->previous_synapse_node != NULL)
                        SMBI->previous_synapse_node->next_element = synapse_node;
                    
                }
                
                // motif has been removed, so add the amount of neurons
                rest_neuron_row += remove_motif_info->n_neurons;

            }

            // if we are in the same column, move to the next as the computation of this column has already finished
            if(synapse_node && synapse_node->next_element != NULL && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                // actual motif has an input motif and the selected one an output motif
                
                
                //printf(" > > > > Moving to the next column!\n");
                while(synapse_node && synapse_node->next_element != NULL && synapse_node->col == SMBI->motifs_first_neuron_indexes[actual_motif_index] + i){
                    
                    synapse_node->col -= rest_neuron_col;
                    synapse_node->row -= rest_neuron_row;

                        //if(synapse_node->row < 0){ printf(" NEGATIVE!!!\n");}
                    SMBI->previous_synapse_node = synapse_node;
                    synapse_node = synapse_node->next_element;
                }

                // if it is the last element, update the column and the row
                if(synapse_node && synapse_node->next_element == NULL){
                    synapse_node->col -= rest_neuron_col;
                    synapse_node->row -= rest_neuron_row;
                        //if(synapse_node->row < 0){ printf(" NEGATIVE!!!\n");}
                }
            }
            // if it is the last element, update the column and the row
            else if(synapse_node && synapse_node->next_element == NULL){
                // actual motif has an input motif and the selected one an output motif

                synapse_node->col -= rest_neuron_col;
                synapse_node->row -= rest_neuron_row;
                        //if(synapse_node->row < 0){ printf(" NEGATIVE!!!\n");}
            }
        }
    }
    
    return synapse_node;
}


/**
 * Helper functions
 */
int_array_t* map_IO_motifs_to_input(individual *ind, int n_new_motifs, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs){
    int i, j;

    int_array_t *all_selected_input_motifs;
    int **aux_array, *n_aux;
    int tmp_motif_index;


    /* Allocate memory */
    all_selected_input_motifs = (int_array_t *)malloc(ind->n_motifs * sizeof(int_array_t));
    aux_array = (int **)malloc(ind->n_motifs * sizeof(int *));
    
    for(i = 0; i<ind->n_motifs; i++)
        aux_array[i] = (int *)malloc(ind->n_motifs * 5 * sizeof(int)); // TODO:the size of this should not be fixed
    n_aux = (int *)calloc(ind->n_motifs, sizeof(int));


    /* Map */

    // Map from input-output lists to only a global input list
    // loop over new motifs for maping output motifs to input motifs 
    for(i = 0; i<n_new_motifs; i++){
        
        // loop over output motifs of this motif
        for(j = 0; j<selected_output_motifs[i].n; j++){
            tmp_motif_index = selected_output_motifs[i].array[j];
            
            // if the selected one is the same as the actual one, then it will appear in the input motifs too, so there will be added
            //if(tmp_motif_index != i){
                aux_array[tmp_motif_index][n_aux[tmp_motif_index]] = i + ind->n_motifs - n_new_motifs; 
                n_aux[tmp_motif_index] ++;
            //}

            
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

    // free allocated memory
    free(n_aux);
    for(i = 0; i<ind->n_motifs; i++)
        free(aux_array[i]); 
    free(aux_array);

    return all_selected_input_motifs;
}

/* Map from a int_array that contains input motif for a set of motifs, to output motifs */
int_array_t *map_from_input_motifs_to_output(individual *ind, int n_motifs, int n_new_motifs, int max_connected_motifs, int_array_t *all_selected_input_motifs){
    int i, j, tmp_motif_index;

    int_array_t *all_selected_output_motifs = (int_array_t *)calloc(ind->n_motifs, sizeof(int_array_t));

    for(i = 0; i<ind->n_motifs; i++){        
        
        // loop over input motifs of this motif
        for(j = 0; j<all_selected_input_motifs[i].n; j++){

            tmp_motif_index = all_selected_input_motifs[i].array[j];

            if(!all_selected_output_motifs[tmp_motif_index].array){
                all_selected_output_motifs[tmp_motif_index].n = 0;
                all_selected_output_motifs[tmp_motif_index].array = (int *)calloc(max_connected_motifs * n_new_motifs * 100, sizeof(int)); // TODO: hmmmm
            }

            all_selected_output_motifs[tmp_motif_index].array[all_selected_output_motifs[tmp_motif_index].n] = i;
            all_selected_output_motifs[tmp_motif_index].n ++;
        }
    } 

    return all_selected_output_motifs;
}


/* Learning zones mutations */

// Function to add new learning zones
void add_learning_zone_mutation(NSGA2Type *nsga2Params, individual *ind){

    int n_new_lz, i, tmp_motif_index, n_layers, dir;
    new_motif_t **motifs_array, *tmp_motif_node;
    learning_zone_t *lz;

    // copy motifs from dynamic list to an array for efficiency
    motifs_array = (new_motif_t **)calloc(ind->n_motifs, sizeof(new_motif_t *));
    new_motif_t *motif_node = ind->motifs_new;

    // initialize helper arrays // TODO: Probably in this second phase the array of motifs can be helpful all the time, do it only once
    for(i = 0; i<ind->n_motifs; i++){
        motifs_array[i] = motif_node;
        motif_node->in_lz = 0; 
        motif_node = motif_node->next_motif;
    }

    // loop over the last learning zone
    lz = ind->learning_zones;
    while(lz->next_zone)
        lz = lz->next_zone; // It would be helpfull to store the last one to get it directly

    
    // select how much learning zones will be added
    n_new_lz = rnd(1, 1); // TODO: This should be a fucntion that depends on the number of learning zones in the individual

    // select the centoid for the new learning zone (a motif that it is not already a centroid)
    i = 0;
    while(i < n_new_lz){ // TODO: check that it is possible to be impossible the creation of the selected amount of learning zones
        // select the centroid motif for starting propagating the learning zone
        tmp_motif_index = rnd(0, ind->n_motifs-1);

        // check if the selected motifs has been selected for another learning zone in this generation
        tmp_motif_node = motifs_array[tmp_motif_index];
        
        if(tmp_motif_node->in_lz == 0){

            // configure new learning zone
            n_layers = rnd(1, 1); // TODO
            dir = rnd(-1, 1); // direction to expand the learning zone

            // initialize learning zone
            lz = initialize_and_allocate_learning_zone(nsga2Params, ind, lz, lz);

            // add tmp motif node to the new learning zone
            add_motif_to_learning_zone(nsga2Params, ind, lz, ind->n_learning_zones-1, tmp_motif_node);

            // expand learning zone
            extend_learning_zone(nsga2Params, ind, lz, ind->n_learning_zones - 1, tmp_motif_index, motifs_array, n_layers, dir);
            i++;
        }
    }

    
    // deallocate memory
    free(motifs_array);
}

// Function to extend an existing learning zone
void extend_learning_zones_mutation(NSGA2Type *nsga2Params, individual *ind){

    int i, n_lz, motif_index, dir, n_layers;
    new_motif_t **motifs_array, *motif_node;
    
    // copy motifs from dynamic list to an array for efficiency
    motifs_array = (new_motif_t **)calloc(ind->n_motifs, sizeof(new_motif_t *));
    motif_node = ind->motifs_new;

    // initialize helper arrays // TODO: Probably in this second phase the array of motifs can be helpful all the time, do it only once
    for(i = 0; i<ind->n_motifs; i++){
        motifs_array[i] = motif_node;
        motif_node->in_lz = 0; 
        motif_node = motif_node->next_motif;
    }


    // select how much learning zones will be changed
    n_lz = rnd(1,1); // TODO //rnd(1, ind->n_learning_zones / 4);
    if(n_lz < 1) 
        n_lz = 1;

    // select motifs, and extend that learning zone in the selected direction
    i=0;
    while(i < n_lz){
        
        // select motif to use as center for learning zone propagation
        motif_index = rnd(0, ind->n_motifs-1);
        motif_node = motifs_array[motif_index];

        if(motif_node->in_lz == 0){ // if motif is not already added to a learning zone in this mutation, then mutate
            
            // select the direction to extend the learning zone
            dir = rnd(-1, 1);
            // select how much layers will be extended
            n_layers = rnd(1, 1); // TODO: This should be dependent on connectivity and number of motifs in the individual
            
            // extend learning layer
            extend_learning_zone(nsga2Params, ind, motif_node->lz, motif_node->lz_index, motif_index, motifs_array, n_layers-1, dir);

            i++;            
        }
    }


    // deallocate memory
    free(motifs_array);
}

void change_learning_rule_mutation(NSGA2Type *nsga2Params, individual *ind){

    int i, j, n_changes, *lz_indexes;
    learning_zone_t *lz;
    new_motif_t **motifs_array;

    // select how much learning zones will be changed
    n_changes = rnd(1, 1);

    // array to store the indexes of those learning zones to change
    lz_indexes = (int *)calloc(n_changes, sizeof(int));

    // initializations for looping
    j = 0;
    i = 0;
    lz = ind->learning_zones;
    while(lz && j < n_changes){
        
        if(i == lz_indexes[j] && lz->n_motifs > 0){
            lz->lr[0] = rnd(nsga2Params->min_learning_rule, nsga2Params->max_learning_rule); // TODO: revise for cases in which more than one learning rules are allowed
            j++;
        }

        lz = lz->next_zone;
        i++;
    }
}

/*int_array_t* select_learning_zones(individual *ind){
    
    int n_learning_zones, min, max;

    // decide how much learning zones to change
    min = 1;
    max = ind->n_motifs * 0.1;

    if(max < min) 
        max = 1;
    
    max = 1; // TODO: This is temporal
    n_learning_zones = rnd(min, max);

    // select n_learning_zones

    int i = 0, j=0, valid = 0;
    int_array_t *learning_zones_to_change;
    learning_zones_to_change = (int_array_t *)calloc(1 + n_learning_zones, sizeof(int_array_t)); // first int_array indicates the selected learning zones,
                                                                                                 // the rest the motif

    learning_zones_to_change->n = n_learning_zones;
    learning_zones_to_change = (int *)calloc(n_learning_zones, sizeof(int));

    while(i < n_learning_zones){
        valid = 1;
        learning_zones_to_change->array[i] = rnd(0, ind->n_learning_zones-1);
    
        for(j = 0; j<i; j++){
            if(learning_zones_to_change->array[j] == learning_zones_to_change->array[i])
                valid = 0;
        }

        if(valid == 1)
            i++;
    }


    // select the number of motifs to add per learning zone

    return learning_zones_to_change;
}*/


/**
 * Functions not used by me // TODO: Remove in the future
 */

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
