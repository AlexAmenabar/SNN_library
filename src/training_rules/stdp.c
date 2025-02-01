#include "snn_library.h"
#include "training_rules/stdp.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define TAU_PLUS 20 // ms
#define TAU_MINUS 20 // ms
#define A_PLUS 0.005 // ms
#define A_MINUS 0.005 // ms
#define A 0.25 // modulation magnitude for STDP

void add_stdp(synapse_t *synapse){
    if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
        int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

        int excitatory; // Set this as synapse parameter to avoid this if
        if(synapse->w>0) excitatory = 1;
        else excitatory = 0;

        if(time_diff > 0){
            synapse->w += A_PLUS * exp(-time_diff / TAU_PLUS);
        }
        else if(time_diff < 0){
            synapse->w -= A_MINUS * exp(time_diff / TAU_MINUS);
        }

        // SET MAX AND MIN VALUES?
        if(excitatory==1 && synapse->w < 0)
            synapse->w = 0.00001;
        else if(excitatory == 0 && synapse->w > 0)
            synapse->w = -0.00001;
    }
}

void mult_stdp(synapse_t *synapse){
    if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
        int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

        int excitatory;
        if(synapse->w>0) excitatory = 1;
        else excitatory = 0;

        if(time_diff > 0){
            synapse->w += A_PLUS * synapse->w * (1 - synapse->w) * exp(-time_diff / TAU_PLUS);
        }
        else if(time_diff < 0){
            synapse->w -= A_MINUS * synapse->w * (1 - synapse->w) * exp(time_diff / TAU_MINUS);
        }

        // SET MAX AND MIN VALUES?
        if(excitatory==1 && synapse->w < 0)
            synapse->w = 0.00001;
        else if(excitatory == 0 && synapse->w > 0)
            synapse->w = -0.00001;
    }
}

void anti_stdp(synapse_t *synapse){
    if(synapse->t_last_pre_spike != -1 && synapse->t_last_post_spike != -1 && synapse->t_last_post_spike != synapse->t_last_pre_spike){
        int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

        int excitatory; // Set this as synapse parameter to avoid this if
        if(synapse->w>0) excitatory = 1;
        else excitatory = 0;

        if(time_diff > 0){
            synapse->w -= A_PLUS * exp(-time_diff / TAU_PLUS);
        }
        else if(time_diff < 0){
            synapse->w += A_MINUS * exp(time_diff / TAU_MINUS);
        }

        // SET MAX AND MIN VALUES?
        if(excitatory==1 && synapse->w < 0)
            synapse->w = 0.00001;
        else if(excitatory == 0 && synapse->w > 0)
            synapse->w = -0.00001;
    }
}

// TODO
void triplet_stdp(synapse_t *synapse){

}
