#include "snn_library.h"
#include "neuron_models/neuron_models.h"
#include "neuron_models/lif_neuron.h"

void step_lif_neuron(spiking_nn_t *snn, int t, int neuron_id, unsigned char **generated_spikes){
    lif_neuron_t *lif_neuron = &(snn->lif_neurons[neuron_id]);
    int i;
    float input_current = 0;

#ifdef DEBUG
    printf(" - Processing neuron %d \n", neuron_id);
#endif

    // check all input synapses to find if there are spikes to process
    for(i=0; i<lif_neuron->n_input_synapse; i++)
    {
        int synapse_index = lif_neuron->input_synapse_indexes[i]; // get synapse
        synapse_t *synapse = &(snn->synapses[synapse_index]);

#ifdef DEBUG
        printf(" -- Processing input synapse %d: ", synapse_index);
#endif

        int next_spike_time; // next spike to process
        next_spike_time = synapse->l_spike_times[synapse->next_spike];

        // if next spike time is equals to actual time step, process
        if(next_spike_time == t){
#ifdef DEBUG
            printf(" SPIKE");
#endif
            // add spike time to neuron received spikes list
            input_current += synapse->w;

            // refresh next spike index
            synapse->next_spike = (synapse->next_spike + 1) % synapse->max_spikes;
        }

        // indicate that this neuron has been processed to synapse
        synapse->post_neuron_computed = 1;

        // compute STDP if both neurons connected to this synapse have been processed
        if(synapse->post_neuron_computed == 1 && synapse->pre_neuron_computed == 1){
            synapse->post_neuron_computed = 0;
            synapse->pre_neuron_computed = 0;

            // compute STDP
            synapse->learning_rule(synapse); // rarete
        }

#ifdef DEBUG
        printf("\n");
#endif
    }

    float temp_v = lif_neuron->v;
    // compute membrane potential
    //lif_neuron->v = lif_neuron->v * (1.0 - DT / TC) + DT / TC * lif_neuron->r * input_current;
    lif_neuron->v = lif_neuron->v + (DT / TC) * (-(lif_neuron->v - lif_neuron->v_rest)) + lif_neuron->r * input_current;

#ifdef DEBUG
    printf(" -- Processing membrane potential: \n --- Initial v: %f \n --- Input current: %f \n --- Actual v: %f \n --- calculation: %f\n", temp_v, input_current, lif_neuron->v, (DT / TC) * (-(lif_neuron->v - lif_neuron->v_rest)));
#endif

#ifdef DEBUG
    printf(" -- Checking if spike must be generated: v = %f, v_tresh = %f\n", lif_neuron->v, lif_neuron->v_tresh);
#endif

    // generate spike if it is necessary
    if(lif_neuron->v >= lif_neuron->v_tresh){
#ifdef DEBUG
        printf(" -- Spike generated by %d at time %d\n", neuron_id, t);
 #endif

        // send spike to all output synapses
        for(i = 0; i<lif_neuron->n_output_synapse; i++){
            int synapse_index = lif_neuron->output_synapse_indexes[i]; // get synapse index
            synapse_t *synapse = &(snn->synapses[synapse_index]);

#ifdef DEBUG
            printf(" --- Processing output synapse %d: spike located for time %d\n", synapse_index, t + synapse->delay);
#endif

            // add spike to output synapse
            synapse->l_spike_times[synapse->last_spike] = t + synapse->delay;

            // refresh last spike index
            synapse->last_spike = (synapse->last_spike + 1) % synapse->max_spikes;

            // register spike time on output synapses
            synapse->t_last_pre_spike = t; // neuron is pre synaptic neuron for output synapses

            // indicate that the neuron has been processed (this is pre neuron for output synapses)
            synapse->pre_neuron_computed = 1;

            // compute STDP if both neurons connected to this synapse have been processed
            if(synapse->post_neuron_computed == 1 && synapse->pre_neuron_computed == 1){
                synapse->post_neuron_computed = 0;
                synapse->pre_neuron_computed = 0;

                // compute STDP
                synapse->learning_rule(synapse); // rarete
            }
        }

        // register spike time on input synapses (NOT COMPATIBLE WITH GPU IMPLEMENTATION???)
        for(i=0; i<lif_neuron->n_input_synapse; i++){
            int synapse_index = lif_neuron->input_synapse_indexes[i]; // get synapse index
            synapse_t *synapse = &(snn->synapses[synapse_index]);

            synapse->t_last_post_spike = t; // this neuron is postsynaptic neuron for input synapses
        }

        lif_neuron->v = lif_neuron->v_reset;


        // for output
        generated_spikes[neuron_id][t] = '|';
    }
    else{
        // indicate that this neuron has been processed to all output synapses
        for(i = 0; i<snn->n_output_synapses; i++){
            // get synapse
            int synapse_index = lif_neuron->output_synapse_indexes[i]; // get synapse index
            synapse_t *synapse = &(snn->synapses[synapse_index]); // get synapse

            // indicate that the neuron has been processed (this is pre neuron for output synapses)
            synapse->pre_neuron_computed = 1;

            // compute STDP if both neurons connected to this synapse have been processed
            if(synapse->post_neuron_computed == 1 && synapse->pre_neuron_computed == 1){
                synapse->post_neuron_computed = 0;
                synapse->pre_neuron_computed = 0;

                // compute STDP
                synapse->learning_rule(synapse); // rarete
            }
        }

        // no spike generated
        generated_spikes[neuron_id][t] = ' ';
    }



#ifdef DEBUG
    printf("\n");
#endif
}


void initialize_lif_neuron(spiking_nn_t *snn, int neuron_index, int excitatory, int *synapse_matrix){
    // Initialize lif neuron parameters
    int i;

    lif_neuron_t *neuron = &(snn->lif_neurons[neuron_index]);
    neuron->v_tresh = -50;//0.25;
    neuron->v_rest = -65;// -70 to -50 mV --> random value on that range?
    neuron->v_reset = -65;
    neuron->v= neuron->v_rest; //0.1;
    neuron->r = 10;
    neuron->excitatory = excitatory;

    // check if neuron is input/output neuron 
    if(neuron_index < snn->n_input)
        neuron->is_input_neuron = 1;
    else
        neuron->is_input_neuron = 0;

    if((snn->n_neurons - snn->n_output) <= neuron_index)
        neuron->is_output_neuron = 1;
    else
        neuron->is_output_neuron = 0;

    // Initialize synapse data
    int n_input_synapse = 0, n_output_synapse = 0;
    
    // count input synapses
    for(i=0; i<snn->n_neurons; i++)
        n_input_synapse += synapse_matrix[i * (snn->n_neurons+1) + neuron_index]; // input synapses are found on the column
   
    // count output synapses
    for(i=1; i<snn->n_neurons+1;i++)
        n_output_synapse += synapse_matrix[(neuron_index + 1) * (snn->n_neurons + 1) + i]; // input synapses are found on the row

    // reserve memory to store synapses indexes
    neuron->input_synapse_indexes = malloc(n_input_synapse * sizeof(int));
    neuron->output_synapse_indexes = malloc(n_output_synapse * sizeof(int));
    neuron->n_input_synapse = n_input_synapse;
    neuron->n_output_synapse = n_output_synapse;

    neuron->next_input_synapse = 0; 
    neuron->next_output_synapse = 0;
}


void add_input_synapse_to_lif_neuron(lif_neuron_t *neuron, int synapse_index){
    neuron->input_synapse_indexes[neuron->next_input_synapse] = synapse_index;
    neuron->next_input_synapse += 1;
}


void add_output_synapse_to_lif_neuron(lif_neuron_t *neuron, int synapse_index){
    neuron->output_synapse_indexes[neuron->next_output_synapse] = synapse_index;
    neuron->next_output_synapse += 1;
}