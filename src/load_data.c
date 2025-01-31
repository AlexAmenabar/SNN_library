#include "../include/snn_library.h"

int open_file(FILE **f, const char *file_name){
    *f = fopen(file_name, "r");
    if (*f == NULL){
        perror("Error opening the file\n");
        return 1;
    }    
    printf("File openned!\n");
    return 0;
}

void close_file(FILE **f){
    fclose(*f);
}

void load_network_information(const char *file_name, int *n_neurons, int *n_input, int *n_output, 
                int *n_synapses, int **synapse_matrix, int **neuron_excitatory, float **weight_list, int **delay_list, int **training_zones) {
    FILE *f = NULL;
    open_file(&f, file_name);

    int i;

    // read number of neurons from file
    fscanf(f, "%d", n_neurons);
    fscanf(f, "%d", n_input);
    fscanf(f, "%d", n_output);
    fscanf(f, "%d", n_synapses);

#ifdef DEBUG
    printf("First parameters readed\n");
#endif


    // reserve memory
    (*neuron_excitatory) = (int *)malloc(*(n_neurons) * sizeof(int));
    *synapse_matrix = (int *)malloc(((*n_neurons) + 1) * ((*n_neurons)+1) * sizeof(int));
    *weight_list = (float *)malloc(*n_synapses * sizeof(float));
    *delay_list = (int *)malloc(*n_synapses * sizeof(int));
    *training_zones = (int *)malloc(*n_synapses * sizeof(int));

#ifdef DEBUG
    printf("Memory reserved\n");
    printf("n = %d, n_i = %d, n_o = %d, n_synap = %d\n", *n_neurons, *n_input, *n_output, *n_synapses);
#endif


    // load available information from file
    if(INPUT_NEURON_BEHAVIOUR == 2) // Load from file
        for(i=0; i<*n_neurons; i++)
            fscanf(f, "%d", &((*neuron_excitatory)[i]));

#ifdef DEBUG
    printf("Input behaviour loaded\n");
#endif

    // load synapses if in file
    if(INPUT_SYNAPSES == 1)
        for(i=0; i<((*n_neurons) + 1) * ((*n_neurons) + 1); i++)
            fscanf(f, "%d", &((*synapse_matrix)[i]));

#ifdef DEBUG
    printf("Synapse matrix loaded\n");
#endif

    // load weights if in file
    if(INPUT_WEIGHTS == 1)
        for(i=0; i<*n_synapses; i++)
            fscanf(f, "%f", &((*weight_list)[i]));

#ifdef DEBUG
    printf("Synapse weights loaded\n");
#endif

    // load delays if in file
    if(INPUT_DELAYS == 2)
        for(i=0; i<*n_synapses; i++)
            fscanf(f, "%d", &((*delay_list)[i]));

#ifdef DEBUG
    printf("Synapse delays loaded\n");
#endif

    // load training zones if in file
    if(INPUT_TRAINING_ZONES == 1)
        for(i=0; i<*n_synapses; i++)
            fscanf(f, "%d", &((*training_zones)[i]));

#ifdef DEBUG
    printf("Training zones loaded\n\n");
#endif

    close_file(&f);
}

void load_input_spike_trains_on_snn(const char *file_name, spiking_nn_t *snn){
    FILE *f = NULL;
    open_file(&f, file_name);

    int i, j, n_spikes;

    // for each input neuron (SYNAPSE??) there is one input spike train
    if(snn->neuron_type == 0) { // lif neurons
        for(i = 0; i<snn->n_input; i++){
            // read number of spikes for i neuron
            fscanf(f, "%d", &n_spikes);

            int synapse_index = snn->lif_neurons[i].input_synapse_indexes[0];

            // load spikes for i neuron
            for(j=0; j<n_spikes; j++){ // CHANGE IF IT WILL BE POSSIBLE TO CONNECT MORE THAN ONE SYNAPSE TO EACH INPUT NEURON
                fscanf(f, "%d", &(snn->synapses[synapse_index].l_spike_times[j]));
            }
            // refresh spikes index for synapse
            snn->synapses[synapse_index].last_spike += n_spikes-1;
            //snn->synapses[0].
        }
    }
    // else{}

    close_file(&f);
}

/*
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
*/