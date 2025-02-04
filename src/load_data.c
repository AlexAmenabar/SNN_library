#include "snn_library.h"
#include "load_data.h"

#include <stdlib.h>
#include <stdio.h>

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

void load_network_information(const char *file_name, int *n_neurons, int *n_input, int *n_output, int *n_synapses, int *n_input_synapses, int *n_output_synapses, 
                int ***synaptic_connections, int **neuron_excitatory, float **weight_list, int **delay_list, int **training_zones) {
    FILE *f = NULL;
    open_file(&f, file_name);

    int i, j;
    int t_n_synapses;
    int connections = 0;

    // read number of neurons from file
    fscanf(f, "%d", n_neurons);
    fscanf(f, "%d", n_input);
    fscanf(f, "%d", n_output);
    fscanf(f, "%d", n_synapses);
    fscanf(f, "%d", n_input_synapses);
    fscanf(f, "%d", n_output_synapses);


#ifdef DEBUG
    printf("First parameters readed\n");
#endif


    // reserve memory
    (*neuron_excitatory) = (int *)malloc(*(n_neurons) * sizeof(int));

    // alloc memory for synapses
    *synaptic_connections = (int **)malloc(((*n_neurons) + 1) * sizeof(int *)); // + 1 as input layer must be taken into account


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
        for(i=0; i<(*n_neurons + 1); i++){
            fscanf(f, "%d", &connections);

            // alloc memory
            (*synaptic_connections)[i] = malloc((connections * 2 + 1) * sizeof(int)); // for each connection the neuron id and the number of synapses must be stored
            (*synaptic_connections)[i][0] = connections;

            for(j = 1; j<connections+1; j+=2)
                fscanf(f, "%d", &((*synaptic_connections)[i][j])); // number of synapses connected to that neuron
                fscanf(f, "%d", &((*synaptic_connections)[i][j+1])); // number of synapses connected to that neuron
        }
            //fscanf(f, "%d", &((*synapse_matrix)[i]));

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

    // the first synapses are input synapses
    for(i = 0; i<snn->n_input_synapses; i++){
        // read number of spikes for i neuron
        fscanf(f, "%d", &n_spikes);

        // load spikes for i neuron
        for(j=0; j<n_spikes; j++)
            fscanf(f, "%d", &(snn->synapses[i].l_spike_times[j]));

        // refresh spikes index for synapse
        snn->synapses[i].last_spike += n_spikes-1;
    }
    // else{}

    close_file(&f);
}