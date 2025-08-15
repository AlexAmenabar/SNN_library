#include "snn_library.h"
#include "training_rules/stdp.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define TAU_PLUS 5 // ms
#define TAU_MINUS 5 // ms
#define A_PLUS 0.01 // ms
#define A_MINUS 0.01 // ms
#define A 0.25 // modulation magnitude for STDP

void add_stdp(synapse_t *synapse){
    //if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
    //    int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

    if(synapse->pre_synaptic_lif_neuron!=NULL && synapse->post_synaptic_lif_neuron!=NULL && 
        synapse->post_synaptic_lif_neuron->t_last_spike != -1 && synapse->pre_synaptic_lif_neuron->t_last_spike != -1 && 
        synapse->post_synaptic_lif_neuron->t_last_spike != synapse->pre_synaptic_lif_neuron->t_last_spike){

        int time_diff = synapse->post_synaptic_lif_neuron->t_last_spike - synapse->pre_synaptic_lif_neuron->t_last_spike;
        double initial_weight = synapse->w;

        if(time_diff > 0 && time_diff < 75){
            synapse->w += A_PLUS * exp(-time_diff / TAU_PLUS);
            
            if(initial_weight > 0 && synapse->w < 0)
                synapse->w = 0.0001;
        }
        else if(time_diff < 0 && time_diff > -200){ // time window to stdp be considered
            synapse->w -= A_MINUS * exp(time_diff / TAU_MINUS);

            if(initial_weight < 0 && synapse->w > 0)
                synapse->w = -0.0001;
        }
    }
}

void mult_stdp(synapse_t *synapse){
    //if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
    //    int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;
    if(synapse->pre_synaptic_lif_neuron!=NULL && synapse->post_synaptic_lif_neuron!=NULL && 
        synapse->post_synaptic_lif_neuron->t_last_spike != -1 && synapse->pre_synaptic_lif_neuron->t_last_spike != -1 && 
        synapse->post_synaptic_lif_neuron->t_last_spike != synapse->pre_synaptic_lif_neuron->t_last_spike){
        
        int time_diff = synapse->post_synaptic_lif_neuron->t_last_spike - synapse->pre_synaptic_lif_neuron->t_last_spike;
        double initial_weight = synapse->w;

        if(time_diff > 0 && time_diff < 75){
            synapse->w += A_PLUS * synapse->w * (1 - synapse->w) * exp(-time_diff / TAU_PLUS);
            
            if(initial_weight > 0 && synapse->w < 0)
                synapse->w = 0.0001;
        }
        else if(time_diff < 0 && time_diff > -200){ // time window to stdp be considered
            synapse->w -= A_MINUS * synapse->w * (1 - synapse->w) * exp(time_diff / TAU_MINUS);

            if(initial_weight < 0 && synapse->w > 0)
                synapse->w = -0.0001;
        }
    }
}

void anti_stdp(synapse_t *synapse){
    //if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
    //    int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

    if(synapse->pre_synaptic_lif_neuron!=NULL && synapse->post_synaptic_lif_neuron!=NULL && 
        synapse->post_synaptic_lif_neuron->t_last_spike != -1 && synapse->pre_synaptic_lif_neuron->t_last_spike != -1 && 
        synapse->post_synaptic_lif_neuron->t_last_spike != synapse->pre_synaptic_lif_neuron->t_last_spike){
        
        int time_diff = synapse->post_synaptic_lif_neuron->t_last_spike - synapse->pre_synaptic_lif_neuron->t_last_spike;
        double initial_weight = synapse->w;

        if(time_diff > 0 && time_diff < 75){
            synapse->w -= A_PLUS * exp(-time_diff / TAU_PLUS);
            
            if(initial_weight > 0 && synapse->w < 0)
                synapse->w = 0.0001;
        }
        else if(time_diff < 0 && time_diff > -200){ // time window to stdp be considered
            synapse->w += A_MINUS * exp(time_diff / TAU_MINUS);

            if(initial_weight < 0 && synapse->w > 0)
                synapse->w = -0.0001;
        }
    }
}

// TODO
void triplet_stdp(synapse_t *synapse){

}
