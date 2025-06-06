/* Mutation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "nsga2.h"
# include "rand.h"


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

    // this should be refactorized
    switch(mutation_code){
        case 0: // change threshold
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 1: // change v_rest
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 2: // change latency
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 3: // change refract time
            neuron_change_mutation(nsga2Params, ind, mutation_code);
            break;
        case 4: // add motif
            break;
        case 5: // remove motif
            break;
        case 6: // connect motifs
            break;
        case 7: // disconnect motifs
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
            case 3: // change threshold
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
int_array_t* select_motifs(individual *ind, int n_motifs){
    int i, j;

    int_array_t *selected_motifs;
    selected_motifs = (int_array_t *)malloc(n_motifs * sizeof(int_array_t)); // selected input/output motifs per each new motif
    

    // per each new motif, select input or output motifs
    for(i = 0; i<n_motifs; i++){
        // select amount of motifs and allocate memory
        selected_motifs[i].n = rnd(1, 1); // TODO: this should not be 1 always
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

/* This mutation adds a new motif (or more than one) to the network (with random connections) */
void add_motif_mutation(NSGA2Type *nsga2Params, individual *ind){
    int i = 0, j;

    int n_new_motifs, last_motif_id; // vairables to store the number of new motifs that are added to the genotype and the id of the last processed motif
    int_array_t *selected_input_motifs, *selected_output_motifs; // lists to store the selected input and output motifs for the new motif
    new_motif_t *tmp_motif_node; // motif nodes to store the new motifs and to loop over the dynamic list of motifs
    sparse_matrix_node_t *synapse_node; // synapse node to loop over the dynamic list of synapses
    neuron_node_t *neuron_node; // neuron node to loop over the dynamic list of neurons and to add new ones

    // select how many motifs will be added
    n_new_motifs = rnd(1, 1); // TODO: it should be possible to add more than one motifs, specially depending on the size of the network

    // add the number of new motifs to the individual
    ind->n_motifs += n_new_motifs;

    // select the motifs that are connected to the new motif and to which motifs is connected the new motif
    selected_input_motifs = select_motifs(ind, n_new_motifs); // select input motifs for each new motif
    selected_output_motifs = select_motifs(ind, n_new_motifs); // selected output motifs for each new motif

    // get the first motif and synapse
    tmp_motif_node = ind->motifs_new; // first motif
    neuron_node = ind->neurons;

    // loop over motifs and locate the new one at the last position
    for(i = 0; i<ind->n_motifs-1; i++)
        tmp_motif_node = tmp_motif_node->next_motif; // loop until the last motif

    // store last motif id, as the new motif id will be the following one
    last_motif_id = tmp_motif_node->motif_id;

    // loop over the network neurons until the last is reached
    for(i = 0; i<ind->n_neurons-1; i++)
        neuron_node = neuron_node->next_neuron;

    // add new motifs, and for each motif, its neurons
    new_motif_t *motif_node = tmp_motif_node;

    for(i = 0; i<n_new_motifs; i++){
        // allocate memory for the next motif (the new one)
        motif_node->next_motif = (new_motif_t *)malloc(sizeof(new_motif_t));
        motif_node = motif_node->next_motif; // move to the next motif in the dynamic list
        initialize_motif_node(motif_node, last_motif_id, ind); // initialize the motif (motif type...)
        
        // add motif neurons to the dynamic list of neurons
        for(j = 0; j<motifs_data[motif_node->motif_type].n_neurons; j++){
            neuron_node->next_neuron = (neuron_node_t *)malloc(sizeof(neuron_node_t)); // allocate memory for the next neuron
            neuron_node = neuron_node->next_neuron; // move to the next neuron 
            initialize_neuron_node(neuron_node); // initialize neuron node
        }

        // update motif id for the next one
        last_motif_id ++;
    }


    /* Initialize synapses */
    synapse_node = ind->connectivity_matrix; // first synapse

    // add the new connections to the sparse matrix
    construct_semi_sparse_matrix(ind, n_new_motifs, motif_node, selected_input_motifs, selected_output_motifs);
}

// This mutation removes a motif from the network
void remove_motif_mutation(NSGA2Type *nsga2Params, individual *ind){
    int i = 0;

    int n_remove_motifs = rnd(1, 1); // TODO: it should be possible to add more than one motifs, specially depending on the size of the network

    int_array_t *selected_motif = select_motifs(ind, n_remove_motifs);
    new_motif_t *motif_node = ind->motifs_new; // fetch first motif
    
}

/* This mutation adds new connections to the network */
void add_connection_mutation(NSGA2Type *nsga2Params, individual *ind){

}

/* Mutation to remove connections from the network */
void remove_connection_mutation(NSGA2Type *nsga2Params, individual *ind){

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
