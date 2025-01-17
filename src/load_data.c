//#include "../include/snn_library.h"


// files load functions
int load_motifs_file(const char *file_name, int *motifs_list, int *number_motifs){
    FILE *f;
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }

    // load number of motifs
    fscanf(f, "%d", number_motifs);
    motifs_list = malloc(*number_motifs * sizeof(int));

    // load list of motifs
    for(int i = 0; i<*number_motifs; i++){
        fscanf(f, "%d", &motifs_list[i]);
    }
}

int load_synapse_matrix(FILE *f, const char *file_name, int *synapse_matrix, int number_neurons){
    f = fopen(file_name, "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }

    for(int i = 0; i<number_neurons; i++)
        for(int j = 0; j<number_neurons; j++)
            fscanf(f, "%d", &synapse_matrix[i*number_neurons + j]);
    return 0;
}

int load_input_spike_train(FILE *f, synapse_t *synapse){
    int spikes = 0;
    fscanf(f, "%d", &spikes);

    for(int i = 0; i<spikes; i++){
        fscanf(f, "%d", &(synapse->l_spike_times[i]));
    }
    synapse->last_spike = spikes-1; // last spike index must be refreshed
    return 0;
}

void load_weight_matrix(){

}

void load_delay_matrix(){

}