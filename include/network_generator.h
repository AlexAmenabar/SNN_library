#ifndef NETWORK_GENERATOR_H
#define NETWORK_GENERATOR_H

void generate_random_synapse_matrix(int *synapse_matrix, int n_neurons, int n_input, int n_output, int *n_synapse, int *n_input_synapse, int *n_output_synapse);
void generate_random_synaptic_weights(float *synaptic_weights, int n_neurons, int *synapse_matrix, int *neuron_behaviour_list);
void generate_random_synaptic_delays(int *synaptic_delays, int n_neurons, int *synapse_matrix);
void generate_random_training_type(int *synaptic_learning_rule, int n_synapses);
void generate_random_neuron_behaviour(int *neurons_behaviour, int n);

#endif