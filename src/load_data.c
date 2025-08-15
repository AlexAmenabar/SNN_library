#include "snn_library.h"
#include "load_data.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <ctype.h>
#include <toml-c.h>

int open_file(FILE **f, const char *file_name){
    *f = fopen(file_name, "r");
    if (*f == NULL){
        perror("Error opening the file\n");
        exit(1);
        return 1;
    }    
    //printf("File openned!\n");
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

void load_network_information(const char *file_name, spiking_nn_t *snn, network_construction_lists_t *lists, simulation_configuration_t *conf) {
    FILE *f = NULL, *f_neurons, *f_synapses;
    char errbuf[100], *file_name_neurons, *file_name_synapses, *original_file_name, *tmp_file_name, *copied_file_name;
    int i, j, l;

    // define table and parameters variables
    toml_table_t *tbl, *tbl_general, *tbl_neurons, *tbl_synapses;

    toml_array_t *behaviour_lst, *v_thres_lst, *t_refract_lst, *input_synapses_lst, 
                *output_synapses_lst, *latency_lst, *weights_lst, *training_zones_lst,
                *connection_lst_lst, *connection_lst;

    toml_value_t n_neurons, n_input_neurons, n_output_neurons, n_synapses, n_input_synapses, n_output_synapses,
                behaviour, v_thres, t_refract, latency, training_zone, weight,
                n_connections, neuron_id, n_synapses_to_neuron, network_is_separated;


    // open TOML file
    open_file(&f, file_name); // TOML file
    printf("File oppened\n");

    // read TOML file
    tbl = toml_parse_file(f, errbuf, 100);
    
    // close the file as the information has been readed
    fclose(f);
    
    printf("Table loaded\n");

    // get sections from file
    tbl_general = toml_table_table(tbl, "general");
    printf("General loaded\n");
   
    tbl_neurons = toml_table_table(tbl, "neurons");
    printf("Neurons loaded\n");

    tbl_synapses = toml_table_table(tbl, "synapsis");
    printf("Synapsis loaded\n");
     
    
    /* 
    General section
    */

    printf("    - Loading general section...\n");
    n_neurons = toml_table_int(tbl_general, "neurons");
    n_input_neurons = toml_table_int(tbl_general, "input_neurons");
    n_output_neurons = toml_table_int(tbl_general, "output_neurons");
    n_synapses = toml_table_int(tbl_general, "synapsis");
    n_input_synapses = toml_table_int(tbl_general, "input_synapsis");
    n_output_synapses = toml_table_int(tbl_general, "output_synapsis");
    network_is_separated = toml_table_int(tbl_general, "network_is_separated");

    // NETWORK IS SEPARATED IS TEMPORAL UNTIL SOMETHING BETTER IS FOUND
    // if network_is_separated == 1 network must be loaded from more than one file
    if(network_is_separated.ok && network_is_separated.u.i == 1){
        // allocate memory for neurons and synapses files
        int len = strlen(file_name);

        file_name_neurons = malloc((len + 100) * sizeof(char));
        file_name_synapses = malloc((len + 100) * sizeof(char));

        // read files
        open_file(&f_neurons, conf->network_neurons_file); // TOML file
        open_file(&f_synapses, conf->network_synapses_file); // TOML file

        /*
        // copy the file name
        int len = strlen(file_name);
        copied_file_name = malloc(len * sizeof(char));
        strcpy(copied_file_name, file_name);

        // split the final .toml
        original_file_name = strtok(copied_file_name, ".toml"); // split file name by extension
        len = strlen(original_file_name);
    
        // allocate memory for neurons and synapses files
        file_name_neurons = malloc((len + 20) * sizeof(char));
        file_name_synapses = malloc((len + 25) * sizeof(char));
    
        // split the "/" to keep the last word and remove path
        tmp_file_name = malloc(len * sizeof(char));
        tmp_file_name = strtok(original_file_name, ""); // split file name by extension

        while(tmp_file_name != NULL){
            tmp_file_name = strtok(NULL, "/")
        }


        strcpy(file_name_neurons, original_file_name);  // copy original file name
        strcpy(file_name_synapses, original_file_name);
    
        strcat(file_name_neurons, "_neurons.toml"); // add extension to original file name for neurons and synapses
        strcat(file_name_synapses, "_synapses.toml");
    
        printf("%s, %s\n", file_name_neurons, file_name_synapses);

        open_file(&f_neurons, file_name_neurons); // TOML file
        open_file(&f_synapses, file_name_synapses); // TOML file
        */
    }


    // check that all the information have been correctly loaded
    if(!(n_neurons.ok && n_input_neurons.ok && n_output_neurons.ok && n_synapses.ok && n_input_synapses.ok && n_output_synapses.ok)){
        printf("The number of neurons, input neurons, output neurons, synapses, input synapses and output synapses must be provided in the network file!");
        exit(1);
    }

    // if correctly readed, store in snn structure
    snn->n_neurons = n_neurons.u.i;
    snn->n_input = n_input_neurons.u.i;
    snn->n_output = n_output_neurons.u.i;
    snn->n_synapses = n_synapses.u.i;
    snn->n_input_synapses = n_input_synapses.u.i;
    snn->n_output_synapses = n_output_synapses.u.i;

#ifdef DEBUG
    printf("n_neurons = %d\n", snn->n_neurons);
    printf("n_input_neurons = %d\n", snn->n_input);
    printf("n_output_neurons = %d\n", snn->n_output);
    printf("n_synapses = %d\n", snn->n_synapses);
    printf("n_input_synapses = %d\n", snn->n_input_synapses);
    printf("n_output_synapses = %d\n", snn->n_output_synapses);
#endif

    // reserve memory for lists related to neurons
    lists->neuron_excitatory = (int *)malloc(snn->n_neurons * sizeof(int));
    lists->r_time_list = (int *)malloc(snn->n_neurons * sizeof(int));
    lists->v_thres_list = (double *)malloc(snn->n_neurons * sizeof(double));
    lists->v_rest_list = (double *)malloc(snn->n_neurons * sizeof(double));

    lists->weight_list = (double *)malloc(snn->n_synapses * sizeof(double));
    lists->delay_list = (int *)malloc(snn->n_synapses * sizeof(int));
    lists->training_zones = (int *)malloc(snn->n_synapses * sizeof(int));

    lists->synaptic_connections = (int **)malloc((snn->n_neurons + 1) * sizeof(int *)); // + 2, one row input layer and the other the output layet

    printf("General section loaded!\n");


    /*
    Neurons section
    */
    printf("    - Loading neurons section...\n");

    behaviour = toml_table_int(tbl_neurons, "behaviour");
    v_thres = toml_table_double(tbl_neurons, "v_thres");
    t_refract = toml_table_int(tbl_neurons, "t_refract");
	 

    // if network information is not separated into more than one files
    if(!network_is_separated.ok || network_is_separated.ok && network_is_separated.u.i != 1){
        behaviour_lst = toml_table_array(tbl_neurons, "behaviour_list");
        v_thres_lst = toml_table_array(tbl_neurons, "v_thres_list");
        t_refract_lst = toml_table_array(tbl_neurons, "t_refract_list");
        input_synapses_lst = toml_table_array(tbl_neurons, "input_synapsis");
        output_synapses_lst = toml_table_array(tbl_neurons, "output_synapsis");

        // load information into snn structure
        for(i=0; i<snn->n_neurons; i++){
            // load data into value variables
            behaviour = toml_array_int(behaviour_lst, i);
            t_refract = toml_array_int(t_refract_lst, i);
            v_thres = toml_array_double(v_thres_lst, i);

            // analyze if this parameters are not provided when it is supposed that they are provided
            // if data was provided load it directly on the network
            if(!behaviour.ok && conf->behaviours_provided == 1 || conf->behaviours_provided == 0){
                printf("Following configuration file, behaviours for neurons must be provided, setting 1 (excitatory)\n");
                behaviour.u.i = 1;
            }
            if(!v_thres.ok && conf->thresholds_provided == 1 || conf->thresholds_provided == 0){
                printf("Following configuration file, thresholds for neurons must be provided, setting 150\n");
                v_thres.u.d = 150;
            }
            if(!t_refract.ok && conf->refract_times_provided == 1 || conf->refract_times_provided == 0){
                printf("Following configuration file, refractary times for neurons must be provided, setting 3\n");
                t_refract.u.i = 3;
            }

            // load data into lists structure
            lists->neuron_excitatory[i] = behaviour.u.i;
            lists->v_thres_list[i] = v_thres.u.d;
            lists->r_time_list[i] = t_refract.u.i; 
        }
    }
    // if it is separated, read the information from the other file
    else{
        for(i=0; i<snn->n_neurons; i++){
            fscanf(f_neurons, "%d", &((lists->neuron_excitatory)[i]));
        }
        for(i=0; i<snn->n_neurons; i++){
            fscanf(f_neurons, "%f", &((lists->v_thres_list)[i]));
        }
        for(i=0; i<snn->n_neurons; i++){
            fscanf(f_neurons, "%f", &((lists->v_rest_list)[i]));
        }
        for(i=0; i<snn->n_neurons; i++){
            fscanf(f_neurons, "%d", &((lists->r_time_list)[i]));
        }
        /*for(i=0; i<snn->n_neurons; i++){
            fscanf(f, "%d", &((behaviour_lst)[i]));
        }
        for(i=0; i<snn->n_neurons; i++){
            fscanf(f, "%d", &((behaviour_lst)[i]));
        }*/
        fclose(f_neurons);
    }
    
    printf("    - Neurons section loaded!\n");

    /*
    Synapses section
    */
    // read synapses section (these list length should be n_synapses)
    //latency = toml_table_int(tbl_synapses, "latency");
    printf("    - Loading synapses section...\n");
    
    // if network information is not separated into more than one files
    if(!network_is_separated.ok || network_is_separated.ok && network_is_separated.u.i != 1){
        latency_lst = toml_table_array(tbl_synapses, "latency_list"); // load latencies
        weights_lst = toml_table_array(tbl_synapses, "weights"); // load weights
        //training_zone = toml_table_int(tbl_synapses, "training_zones");
        training_zones_lst = toml_table_array(tbl_synapses, "training_zones_list"); // load training zones
        connection_lst_lst = toml_table_array(tbl_synapses, "connections"); // load connections
        

        // load information into snn structure
        for(i=0; i<snn->n_synapses; i++){
            // load data into value variables
            latency = toml_array_int(latency_lst, i);
            weight = toml_array_double(weights_lst, i);
            training_zone = toml_array_int(training_zones_lst, i);

            // analyze if this parameters are not provided when it is supposed that they are provided
            // if data was provided load it directly on the network
            if(!latency.ok && conf->delays_provided == 1 || conf->delays_provided == 0){
                printf("Following configuration file, latencies for synapses must be provided, setting 1\n");
                latency.u.i = 1;
            }
            if(!weight.ok && conf->weights_provided == 1 || conf->weights_provided == 0){
                printf("Following configuration file, weights for synapses must be provided, setting 100\n");
                weight.u.d = 100;
            }
            if(!training_zone.ok && conf->training_zones_provided == 1 || conf->training_zones_provided == 0){
                printf("Following configuration file, training zones for neurons must be provided, setting 0 (normal STDP)\n");
                training_zone.u.i = 0;
            }

            // load data into lists structure
            (lists->weight_list)[i] = (double)weight.u.d;
            (lists->delay_list)[i] = (int)latency.u.i;
            (lists->training_zones)[i] = (int)training_zone.u.i;
        }
    }
    // if it is separated, read the information from the other file
    else{
        for(i=0; i<snn->n_synapses; i++){
            fscanf(f_synapses, "%d", &((lists->delay_list)[i]));
        }
        for(i=0; i<snn->n_synapses; i++){
            fscanf(f_synapses, "%f", &((lists->weight_list)[i]));
        }
        for(i=0; i<snn->n_synapses; i++){
            fscanf(f_synapses, "%f", &((lists->training_zones)[i]));
        }
    }


    printf("Loading connections information...\n");

    if(!network_is_separated.ok || network_is_separated.ok && network_is_separated.u.i != 1){
        // load connections (list of lists)
        for(i=0; i<snn->n_neurons+1; i++){
            if(i % 1 == 0) printf("In %d...\n", i);

            connection_lst = toml_array_array(connection_lst_lst, i);
            
            n_connections = toml_array_int(connection_lst, 0);
            
            // check that data has been correctly loaded
            if(!n_connections.ok){
                printf("Connection list is incorrect. Exiting\n");
                exit(1);
            }

            //
            (lists->synaptic_connections)[i] = malloc((n_connections.u.i * 2 + 1) * sizeof(int)); // for each connection the neuron id and the number of synapses must be stored
            (lists->synaptic_connections)[i][0] = n_connections.u.i;
        
            for(int j = 0; j<n_connections.u.i; j++){
                neuron_id = toml_array_int(connection_lst, j * 2 + 1);
                n_synapses_to_neuron = toml_array_int(connection_lst, j * 2 + 2);

                // check that the information have been correctly loaded
                if(!(neuron_id.ok && n_synapses_to_neuron.ok)){
                    printf("Connection list data is incorrect. Exiting\n");
                    exit(1);
                }

                (lists->synaptic_connections)[i][j * 2 + 1] = neuron_id.u.i;
                (lists->synaptic_connections)[i][j * 2 + 2] = n_synapses_to_neuron.u.i;
            } 
        }
    }
    // if network information is separated into more than one file
    else{
        int number_connections;
        for(i=0; i<(snn->n_neurons + 1); i++){ // network input synapses are loaded first and output synapses last
            fscanf(f_synapses, "%d", &number_connections);

            // alloc memory
            (lists->synaptic_connections)[i] = malloc((number_connections * 2 + 1) * sizeof(int)); // for each connection the neuron id and the number of synapses must be stored
            (lists->synaptic_connections)[i][0] = number_connections;

            for(j = 0; j<number_connections; j++){
                fscanf(f_synapses, "%d", &((lists->synaptic_connections)[i][j * 2 + 1])); // number of synapses connected to that neuron
                fscanf(f_synapses, "%d", &((lists->synaptic_connections)[i][j * 2 + 2])); // number of synapses connected to that neuron
            }
        }
        fclose(f_synapses);
    }

    printf("Synapses section loaded\n");
    // Change to TOML

    /*
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
/*
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

    close_file(&f);*/
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

int load_configuration_params_from_toml(const char *file_name, simulation_configuration_t *conf){
    FILE *f = NULL;
    char errbuf[100];
    int i, l;
    int l_file_names = 300;

    // define table and variables to store configuration parameters
    toml_table_t *tbl, *tbl_general, *tbl_simulation, *tbl_samples, *tbl_output, *tbl_network;
    
    toml_value_t execution_type, neuron_type, execution_obj, n_process, learn, 
                time_steps, input_file,
                dataset, dataset_name, num_classes, epochs,
                generated_spikes, final_weights, execution_times, spikes_per_neuron, store_network, store_network_file,
                network, network_neurons, network_synapses, behaviours, delays, weights, training_zones, thresh, t_refract;


    /* open TOML file */
    open_file(&f, file_name); // TOML file

    /* read TOML file */
    tbl = toml_parse_file(f, errbuf, l_file_names);

    // get sections from file
    tbl_general = toml_table_table(tbl, "general");
    tbl_simulation = toml_table_table(tbl, "simulation");
    tbl_samples = toml_table_table(tbl, "samples");
    tbl_output = toml_table_table(tbl, "output");
    tbl_network = toml_table_table(tbl, "network");
        

    /*
    GENERAL SECTION
    */
    execution_type = toml_table_int(tbl_general, "execution_type");
    neuron_type = toml_table_int(tbl_general, "neuron_type");
    execution_obj = toml_table_int(tbl_general, "execution_obj");
    n_process = toml_table_int(tbl_general, "n_process");
    learn = toml_table_int(tbl_general, "learn");

    // if something is missing in configuration file, set default value
    if(!execution_type.ok)
        execution_type.u.i = 0; // clock-based

    if(!neuron_type.ok)
        neuron_type.u.i = 0; // LIF neuron

    if(!execution_obj.ok)
        execution_obj.u.i = 0; // biological simulation

    if(!n_process.ok)
        n_process.u.i = 1; // serial execution

    if(!learn.ok)
        learn.u.i = 1; // learn

    // load information into config structure
    conf->simulation_type = execution_type.u.i;
    conf->neuron_type = neuron_type.u.i;
    conf->n_process = n_process.u.i;
    conf->learn = learn.u.i;
    

    /*
    SIMULATION OR/AND SAMPLES SECTIONS
    */
    if(execution_obj.u.i == 0){
        time_steps = toml_table_int(tbl_simulation, "time_steps");
        input_file = toml_table_string(tbl_simulation, "input_file");

        // if something is missing in configuration file, set default value
        if(!time_steps.ok)
        {
            printf("Amount of time-steps haven't been provided, setting default value: 1000\n");
            time_steps.u.i = 1000;
        }

        if(!input_file.ok){
            printf("A input file must be provided for input spikes!\n");
            exit(1);
        }

        // load information into config structure
        conf->time_steps = time_steps.u.i;

        // allocate memory for string and store into config structure
        l = input_file.u.sl;
        conf->input_spikes_file = malloc(l * sizeof(char));
        conf->input_spikes_file = input_file.u.s;
    }
    else{ // by samples
        ;
    }


    /*
    OUTPUT SECTION
    */
    generated_spikes = toml_table_string(tbl_output, "generated_spikes");
    final_weights = toml_table_string(tbl_output, "final_weights");
    execution_times = toml_table_string(tbl_output, "execution_times");
    spikes_per_neuron = toml_table_string(tbl_output, "spikes_per_neuron");
    store_network = toml_table_int(tbl_output, "store_network");
    store_network_file = toml_table_string(tbl_output, "store_network_file");

    // check that everything is loaded
    if(!generated_spikes.ok){
        printf("A file to store generated spikes must be provided!\n");
        exit(1);
    }
    if(!final_weights.ok){
        printf("A file to store final network files must be provided!\n");
        exit(1);
    }
    if(!execution_times.ok){
        printf("A file to store execution times must be provided!\n");
        exit(1);
    }
    if(!spikes_per_neuron.ok){
        printf("A file to store spikes generated per neuron must be provided!\n");
        exit(1);
    }

    if(!store_network.ok){
        store_network.u.i = 0; // default option
    }
    else if(store_network.u.i == 1 && !store_network_file.ok){
        printf("The file name to store the final network must be provided!\n");
        exit(1);
    }


    // allocate memory for strings and store them into lists structure
    l = generated_spikes.u.sl;
    conf->spike_times_file = malloc(l * sizeof(char));
    conf->spike_times_file = generated_spikes.u.s;

    l = final_weights.u.sl;
    conf->weights_file = malloc(l * sizeof(char));
    conf->weights_file = final_weights.u.s;

    l = execution_times.u.sl;
    conf->times_file = malloc(l * sizeof(char));
    conf->times_file = execution_times.u.s;

    l = spikes_per_neuron.u.sl;
    conf->n_spikes_file = malloc(l * sizeof(char));
    conf->n_spikes_file = spikes_per_neuron.u.s;

    conf->store = store_network.u.i;
    if(conf->store == 1){
        l = store_network_file.u.sl;
        conf->final_network_file = malloc(l * sizeof(char));
        conf->final_network_file = store_network_file.u.s;
    }

    /*
    NETWORK SECTION
    */
    network = toml_table_string(tbl_network, "network_file");
    network_neurons = toml_table_string(tbl_network, "network_neurons_file");
    network_synapses = toml_table_string(tbl_network, "network_synapses_file");
    behaviours = toml_table_int(tbl_network, "behaviours");
    delays = toml_table_int(tbl_network, "delays");
    weights = toml_table_int(tbl_network, "weights");
    training_zones = toml_table_int(tbl_network, "training_zones");
    thresh = toml_table_int(tbl_network, "thresh");
    t_refract = toml_table_int(tbl_network, "t_refract");
    // TODO: more parameters should be added in the future

    // by default these data is not provided, only network file must be provided

    // if something is missing in configuration file, set default value
    if(!network.ok)
    {
        printf("The file to load the network must be provided\n");
        exit(1);
    }

    // if there is not in the file not provided
    if(!behaviours.ok){
        behaviours.u.i = 0;
    }
    
    if(!delays.ok){
        delays.u.i = 0;
    }

    if(!weights.ok){
        weights.u.i = 0;
    }    
    
    if(!training_zones.ok){
        training_zones.u.i = 0;
    }    
    
    if(!thresh.ok){
        thresh.u.i = 0;
    }    

    if(!t_refract.ok){
        t_refract.u.i = 0;
    }    
    
    // load information into config structure
    l = network.u.sl;
    conf->network_file = malloc(l * sizeof(char));
    conf->network_neurons_file = malloc(l * sizeof(char));
    conf->network_synapses_file = malloc(l * sizeof(char));
    conf->network_file = network.u.s;
    if(network_neurons.ok)
        conf->network_neurons_file = network_neurons.u.s;
    if(network_synapses.ok)
        conf->network_synapses_file = network_synapses.u.s;
    
    conf->behaviours_provided = behaviours.u.i;
    conf->delays_provided = delays.u.i;
    conf->weights_provided = weights.u.i;
    conf->training_zones_provided = training_zones.u.i;
    conf->thresholds_provided = thresh.u.i;
    conf->refract_times_provided = t_refract.u.i;
}



// Functions to store results and data into files
void store_results(simulation_results_t *results, simulation_configuration_t *conf, spiking_nn_t *snn){
    int i,j;

    // TODO: Now this function only stores the first sample results
    simulation_results_per_sample_t *results_per_sample = &(results->results_per_sample[i]);

    FILE *output_file;
    output_file = fopen(conf->spike_times_file, "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    for (i = 0; i<snn->n_neurons; i++)
    {
        for(j = 0; j<conf->time_steps; j++)
            fprintf(output_file, "%c", results_per_sample->generated_spikes[i][j]);
        
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
    fprintf(output_file, "%f ", results_per_sample->elapsed_time);
    fprintf(output_file, "%f ", results_per_sample->elapsed_time_neurons);
    fprintf(output_file, "%f \n", results_per_sample->elapsed_time_neurons_input);
    fprintf(output_file, "%f \n", results_per_sample->elapsed_time_neurons_output);
    fprintf(output_file, "%f \n", results_per_sample->elapsed_time_synapses);
    fclose(output_file);
}