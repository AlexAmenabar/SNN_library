#include "../include/snn_library.h"

int open_file(FILE *f, const char *file_name){
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }    
    //printf("File correctly openned!\n");
    return 0;
}

void close_file(FILE *f){
    fclose(f);
}

// FILES MUST BE CLSOED --> RECORDATORIO

// files load functions
void load_motifs_file(const char *file_name, int *motifs_list, int *number_motifs){
    FILE *f;
    open_file(f, file_name);

    // load number of motifs
    fscanf(f, "%d", number_motifs);
    motifs_list = malloc(*number_motifs * sizeof(int));

    // load list of motifs
    for(int i = 0; i<*number_motifs; i++){
        fscanf(f, "%d", &motifs_list[i]);
    }
}

void load_input_spike_train(FILE *f, synapse_t *synapse){
    //open_file(f, file_name);

    int spikes = 0;
    fscanf(f, "%d", &spikes);

    //printf("Amount of spikes readed\n");

    for(int i = 0; i<spikes; i++){
        fscanf(f, "%d", &(synapse->l_spike_times[i]));
        //printf("Ya peto\n");
    }
    synapse->last_spike = spikes-1; // last spike index must be refreshed
}

int load_synapse_matrix(const char *file_name, int *synapse_matrix, int *n_synapses, int n_neurons){
    // open file
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }

    // read synapses file
    *n_synapses = 0;
    int i;
    for(i = 0; i<n_neurons; i++) {
        for(int j = 0; j<n_neurons; j++) {
            fscanf(f, "%d", &synapse_matrix[i*n_neurons + j]);
            *n_synapses += synapse_matrix[i*n_neurons + j]; // count synapse amount
        }
    }
    fclose(f);
    return 0;
}

int load_weight_matrix(const char *file_name, float *weight_matrix, float *input_weights, float *output_weights, int n_synapses, int n_input, int n_output){
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL) {
        perror("Error opening the file\n");
        return 1;
    }

    int i;
    //for(i = 0; i<n_neurons; i++)
    //    for(int j = 0; j<n_neurons; j++)
    //        fscanf(f, "%f", &weight_matrix[i*n_neurons + j]);
    for(i=0; i<n_synapses; i++){
        fscanf(f, "%f", &weight_matrix[i]);
    }

    // THIS WILL BE CHANGED IN THE FUTURE
    for(i=0; i<n_input; i++)
        fscanf(f, "%f", &input_weights[i]);

    for(i=0; i<n_output; i++)
        fscanf(f, "%f", &output_weights[i]);

    return 0;
}

int load_delay_matrix(const char *file_name, int *delay_matrix, int *input_delays, int *output_delays, int n_synapses, int n_input, int n_output){
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }
    //delay_matrix = malloc(n_synapses * sizeof(int));
    //input_delays = malloc(n_input * sizeof(int));
    //output_delays = malloc(n_output * sizeof(int));

    int i;
    for(i = 0; i<n_synapses; i++)
        //for(int j = 0; j<n_neurons; j++)
        fscanf(f, "%d", &delay_matrix[i]);

    for(i=0; i<n_input; i++)
        fscanf(f, "%f", &input_delays[i]);

    for(i=0; i<n_output; i++)
        fscanf(f, "%f", &output_delays[i]);

    return 0;
}

int load_excitatory_neurons(const char *file_name, int *neuron_excitatory, int n_neurons){
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }
    
    int i;
    for(i = 0; i<n_neurons; i++)
        fscanf(f, "%d", &neuron_excitatory[i]);

    return 0;
}