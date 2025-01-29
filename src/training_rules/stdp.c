#include <math.h>
#include "../../include/snn_library.h"
#include "../../include/training_rules/stdp.h"

#define TAU_PLUS 20 // ms
#define TAU_MINUS 20 // ms
#define A_PLUS 0.005 // ms
#define A_MINUS 0.005 // ms
#define A 0.25 // modulation magnitude for STDP

void add_stdp(synapse_t *synapse){
    int time_diff = synapse->t_last_post_spike - synapse->t_last_pre_spike;

    int excitatory; // Set this as synapse parameter to avoid this if
    if(synapse->w>0) excitatory = 1;
    else excitatory = 0;


    if(time_diff > 0){
        synapse->w += A_PLUS * exp(-time_diff / TAU_PLUS);
    }
    else if(time_diff < 0){
        synapse->w -= A_MINUS * exp(-time_diff / TAU_MINUS);
    }

    // SET MAX AND MIN VALUES?
    if(excitatory==1 && synapse->w < 0)
        synapse->w = 0;
    else if(excitatory == 0 && synapse->w > 0)
        synapse->w = 0;
}

void mult_stdp(synapse_t *synapse, int t_pre, int t_post){

}

void anti_stdp();

void triplet_stdp();
