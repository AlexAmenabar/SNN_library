#include "snn_library.h"
#include "load_data.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <ctype.h>

int open_file(FILE **f, const char *file_name){
    *f = fopen(file_name, "r");
    if (*f == NULL){
        perror("Error opening the file\n");
        return 1;
    }    
    printf("File openned!\n");
    return 0;
}

int open_file_w(FILE **f, const char *file_name){
    *f = fopen(file_name, "a"); // append mode, no overwriting
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

void load_network_information(const char *file_name, spiking_nn_t *snn, network_construction_lists_t *lists) {
    FILE *f = NULL;
    open_file(&f, file_name);

    int i, j;
    int connections = 0;

    // read number of neurons from file
    fscanf(f, "%d", &(snn->n_neurons));
    fscanf(f, "%d", &(snn->n_input));
    fscanf(f, "%d", &(snn->n_output));
    fscanf(f, "%d", &(snn->n_synapses));
    fscanf(f, "%d", &(snn->n_input_synapses));
    fscanf(f, "%d", &(snn->n_output_synapses));


#ifdef DEBUG
    printf("First parameters readed\n");
#endif

    /*printf("Memory needs:\n");
    printf(" - Neurons informations: %d\n", *(n_neurons) * sizeof(int));
    printf(" - Synaptic connections: %d\n", ((*n_neurons) + 1) * sizeof(int *));
    printf(" - Weights: %d\n", *(n_neurons) * sizeof(int));
    printf(" - Neurons informations: %d\n", *(n_neurons)*sizeof(int));
    printf(" - Neurons informations: %d\n", *(n_neurons)*sizeof(int));*/

    // reserve memory
    lists->neuron_excitatory = (int *)malloc(snn->n_neurons * sizeof(int));
    printf("Memory reserved\n");

    // alloc memory for synapses
    lists->synaptic_connections = (int **)malloc((snn->n_neurons + 1) * sizeof(int *)); // + 2, one row input layer and the other the output layet
    printf("Memory reserved\n");


    lists->weight_list = (float *)malloc(snn->n_synapses * sizeof(float));
    printf("Memory reserved\n");

    lists->delay_list = (int *)malloc(snn->n_synapses * sizeof(int));
    printf("Memory reserved\n");

    lists->training_zones = (int *)malloc(snn->n_synapses * sizeof(int));
    printf("Memory reserved\n");


//#ifdef DEBUG
    printf("Memory reserved\n");
    printf("n = %d, n_i = %d, n_o = %d, n_synap = %d\n", snn->n_neurons, snn->n_input, snn->n_output, snn->n_synapses);
//#endif


    // load available information from file
    if(INPUT_NEURON_BEHAVIOUR == 2) // Load from file
        for(i=0; i<snn->n_neurons; i++)
            fscanf(f, "%d", &((lists->neuron_excitatory)[i]));

//#ifdef DEBUG
    printf("Input behaviour loaded\n");
//#endif

    // load synapses if in file
    if(INPUT_SYNAPSES == 1)
        for(i=0; i<(snn->n_neurons + 1); i++){ // network input synapses are loaded first and output synapses last
            fscanf(f, "%d", &connections);

            // alloc memory
            (lists->synaptic_connections)[i] = malloc((connections * 2 + 1) * sizeof(int)); // for each connection the neuron id and the number of synapses must be stored
            (lists->synaptic_connections)[i][0] = connections;

            for(j = 0; j<connections; j++){
                fscanf(f, "%d", &((lists->synaptic_connections)[i][j * 2 + 1])); // number of synapses connected to that neuron
                fscanf(f, "%d", &((lists->synaptic_connections)[i][j * 2 + 2])); // number of synapses connected to that neuron
            }
        }
            //fscanf(f, "%d", &((*synapse_matrix)[i]));

//#ifdef DEBUG
    printf("Synapse matrix loaded\n");
//#endif

    // load weights if in file
    if(INPUT_WEIGHTS == 1)
        for(i=0; i<snn->n_synapses; i++)
            fscanf(f, "%f", &((lists->weight_list)[i]));

//#ifdef DEBUG
    printf("Synapse weights loaded\n");
//#endif

    // load delays if in file
    if(INPUT_DELAYS == 2)
        for(i=0; i<snn->n_synapses; i++)
            fscanf(f, "%d", &((lists->delay_list)[i]));

//#ifdef DEBUG
    printf("Synapse delays loaded\n");
//#endif

    // load training zones if in file
    if(INPUT_TRAINING_ZONES == 1)
        for(i=0; i<snn->n_synapses; i++)
            fscanf(f, "%d", &((lists->training_zones)[i]));

//#ifdef DEBUG
    printf("Training zones loaded\n\n");
//#endif

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
        snn->synapses[i].last_spike += n_spikes;
    }
    // else{}

    close_file(&f);
}

/*void insert_at_file_init(const char *file_name, const char *temp_file_name) {
    FILE *f = fopen(file_name, "r");
    if (!f) {
        perror("Error abriendo el archivo original");
        return;
    }

    FILE *temp_file = fopen("temp_file_name", "w");
    if (!temp_file) {
        perror("Error creando archivo temporal");
        fclose(f);
        return;
    }

    // Escribir primero el nuevo contenido en el archivo temporal
    fputs(f, temp_file);

    // Copiar el contenido original al archivo temporal
    char buffer[1024];
    size_t bytes_leidos;
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        fwrite(buffer, 1, bytes_leidos, temp_file);
    }

    fclose(f);
    fclose(temp_file);

    // Reemplazar el archivo original con el archivo temporal
    remove(file_name);
    rename(temp_file_name, file_name);
}*/

int read_file_name(FILE *f, int max_length, char *file_name){
    char ch;
    int length = 0;

    // skip any leading whitespace
    while ((ch = fgetc(f)) != EOF && isspace(ch));

    // read word and count number of characters
    while (ch != EOF && !isspace(ch) && length < max_length) {
        file_name[length] = ch;
        ch = fgetc(f);
        length++;
    }

    printf("Length of first word: %d\n", length);
    printf("Word: %s\n", file_name);
    if(length >= max_length) return 1; // error

    return 0;
}

int load_configuration_params(const char *file_name, simulation_configuration_t *conf){
    FILE *f = NULL;

    int err = 0;

    // allocate memory for file names
    conf->network_file = malloc(100 * sizeof(char));
    conf->spike_times_file = malloc(100 * sizeof(char));
    conf->weights_file = malloc(100 * sizeof(char));
    conf->times_file = malloc(100 * sizeof(char));
    conf->n_spikes_file = malloc(100 * sizeof(char));
    conf->final_network_file = malloc(100 * sizeof(char));

    // read parameters
    open_file(&f, file_name);

    fscanf(f, "%d", &(conf->simulation_type));
    fscanf(f, "%d", &(conf->neuron_type));
    fscanf(f, "%d", &(conf->learn));
    err = read_file_name(f, 100, conf->network_file);
    if(err == 1) 
        return err; //err


    fscanf(f, "%d", &(conf->n_process));
    fscanf(f, "%d", &(conf->store));
    if(conf->store == 1){
        err = read_file_name(f, 100, conf->final_network_file);
        if(err == 1)
            return err; //err
    }

    // output files
    err = read_file_name(f, 100, conf->spike_times_file);
    if(err == 1) 
        return err; //err

    err = read_file_name(f, 100, conf->weights_file);
    if(err == 1) 
        return err; //err

    err = read_file_name(f, 100, conf->times_file);
    if(err == 1) 
        return err; //err

    err = read_file_name(f, 100, conf->n_spikes_file);
    if(err == 1) 
        return err; //err    
    
    // IFDEF SIMULATION...
    conf->input_spikes_file = malloc(100 * sizeof(char));
    err = read_file_name(f, 100, conf->input_spikes_file);
    if(err == 1) 
        return err; //err    
    printf("Hasta aquío bien\n");

    fscanf(f, "%d", &(conf->time_steps));
    printf("Vaya por dios \n");
    // ELSE...
}

// Functions to store results and data into files
void store_results(simulation_results_t *results, simulation_configuration_t *conf, spiking_nn_t *snn){
    int i,j;

    FILE *output_file;
    output_file = fopen(conf->spike_times_file, "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    for (i = 0; i<snn->n_neurons; i++)
    {
        for(j = 0; j<conf->time_steps; j++)
            fprintf(output_file, "%c", results->generated_spikes[i][j]);
        
        fprintf(output_file, "\n");
    }
    fclose(output_file);

    output_file = fopen(conf->weights_file, "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    for (i = 0; i<snn->n_synapses; i++)
    {
        fprintf(output_file, "%f ", snn->synapses[i].w);
        fprintf(output_file, "\n");
    }
    fclose(output_file);

    output_file = fopen(conf->times_file, "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }    
    fprintf(output_file, "%f ", results->elapsed_time);
    fprintf(output_file, "%f ", results->elapsed_time_neurons);
    fprintf(output_file, "%f \n", results->elapsed_time_synapses);
    fclose(output_file);
}