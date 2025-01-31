#ifndef NEURON_MODELS_H
#define NEURON_MODELS_H

void load_network_information(const char *file_name, int *n_neurons, int *n_input, int *n_output, int *n_synapses, int *n_input_synapses, int *n_output_synapses, 
                int **synapse_matrix, int **neuron_excitatory, float **weight_list, int **delay_list, int **training_zones);


void load_input_spike_trains_on_snn(const char *file_name, spiking_nn_t *snn);


#endif