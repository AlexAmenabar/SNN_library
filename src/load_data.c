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
    char errbuf[100];
    int i, l;

    // define table and parameters variables
    toml_table_t *tbl, *tbl_general, *tbl_neurons, *tbl_synapses;
    toml_value_t n_neurons, n_input_neurons, n_output_neurons, n_synapses, n_input_synapses, n_output_synapses;
    
    toml_value_t behaviour, v_thres, t_refract, latency, training_zone, weight;

    toml_array_t *behaviour_lst, *v_thres_lst, *t_refract_lst, *input_synapses_lst, 
                *output_synapses_lst, *latency_lst, *weights_lst, *training_zones_lst,
                *connection_lst_lst, *connection_lst;

    
    // open TOML file
    open_file(&f, file_name); // TOML file


    // read TOML file
    tbl = toml_parse_file(f, errbuf, 100);

    // get sections from file
    tbl_general = toml_table_table(tbl, "general");
    tbl_neurons = toml_table_table(tbl, "neurons");
    tbl_synapses = toml_table_table(tbl, "synapsis");
        
    // read general section data
    n_neurons = toml_table_int(tbl_general, "neurons");
    n_input_neurons = toml_table_int(tbl_general, "input_neurons");
    n_output_neurons = toml_table_int(tbl_general, "output_neurons");
    n_synapses = toml_table_int(tbl_general, "synapsis");
    n_input_synapses = toml_table_int(tbl_general, "input_synapsis");
    n_output_synapses = toml_table_int(tbl_general, "output_synapsis");
    


    // if one of them haven't been correctly readed end process
    if(!(n_neurons.ok && n_input_neurons.ok && n_output_neurons.ok && n_synapses.ok && n_input_synapses.ok && n_output_synapses.ok)){
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
    lists->v_thres_list = (float *)malloc(snn->n_neurons * sizeof(float));
    lists->v_rest_list = (float *)malloc(snn->n_neurons * sizeof(float));

    lists->weight_list = (float *)malloc(snn->n_synapses * sizeof(float));
    lists->delay_list = (int *)malloc(snn->n_synapses * sizeof(int));
    lists->training_zones = (int *)malloc(snn->n_synapses * sizeof(int));

    lists->synaptic_connections = (int **)malloc((snn->n_neurons + 1) * sizeof(int *)); // + 2, one row input layer and the other the output layet

    // read neurons section
    behaviour = toml_table_int(tbl_neurons, "behaviour");
    behaviour_lst = toml_table_array(tbl_neurons, "behaviour_list");
	
    v_thres = toml_table_double(tbl_neurons, "v_thres");
    v_thres_lst = toml_table_array(tbl_neurons, "v_thres_list");
		
    t_refract = toml_table_int(tbl_neurons, "t_refract");
    t_refract_lst = toml_table_array(tbl_neurons, "t_refract_list");
	 
    input_synapses_lst = toml_table_array(tbl_neurons, "input_synapsis");
    output_synapses_lst = toml_table_array(tbl_neurons, "output_synapsis");
	   
    l = toml_array_len(behaviour_lst);
	for (i = 0; i < l; i++){
        printf("  index %d = %ld\n", i, toml_array_int(behaviour_lst, i).u.i);
    }
    printf("\n");

    l = toml_array_len(v_thres_lst);
    for (i = 0; i < l; i++)
        printf("  index %d = %f\n", i, toml_array_double(v_thres_lst, i).u.d);
    printf("\n");

    l = toml_array_len(t_refract_lst);
    for (i = 0; i < l; i++)
        printf("  index %d = %f\n", i, toml_array_double(t_refract_lst, i).u.d);
    printf("\n");

    l = toml_array_len(input_synapses_lst);
    for (i = 0; i < l; i++)
        printf("  index %d = %ld\n", i, toml_array_int(input_synapses_lst, i).u.i);
    printf("\n");

    l = toml_array_len(output_synapses_lst);
    for (i = 0; i < l; i++)
        printf("  index %d = %ld\n", i, toml_array_int(output_synapses_lst, i).u.i);
    printf("\n");

    // load information into snn structure
    for(i=0; i<snn->n_neurons; i++){
        // load data into value variables
        behaviour = toml_array_int(behaviour_lst, i);
        t_refract = toml_array_int(t_refract_lst, i);
        v_thres = toml_array_double(v_thres_lst, i);

        if(!(behaviour.ok && v_thres.ok && t_refract.ok))
            exit(1);

        // load data into lists structure
        lists->neuron_excitatory[i] = behaviour.u.i;
        lists->v_thres_list[i] = v_thres.u.d;
        lists->r_time_list[i] = t_refract.u.i; 
    }
    
    // read synapses section (these list length should be n_synapses)
    latency = toml_table_int(tbl_synapses, "latency");
    latency_lst = toml_table_array(tbl_synapses, "latency_list");

    weights_lst = toml_table_array(tbl_synapses, "weights");
        
    //training_zone = toml_table_int(tbl_synapses, "training_zones");
    training_zones_lst = toml_table_array(tbl_synapses, "training_zones_list");
     
    connection_lst_lst = toml_table_array(tbl_synapses, "connections");
    

    // load information into snn structure
    for(i=0; i<snn->n_synapses; i++){
        // load data into value variables
        latency = toml_array_int(latency_lst, i);
        weight = toml_array_double(weights_lst, i);
        training_zone = toml_array_int(training_zones_lst, i);

        if(!(latency.ok && weight.ok && training_zone.ok))
            exit(1);

        // load data into lists structure
        (lists->weight_list)[i] = (float)weight.u.d;
        (lists->delay_list)[i] = (int)latency.u.i;
        (lists->training_zones)[i] = (int)training_zone.u.i;
    }

    for(i=0; i<snn->n_neurons+1; i++){
        connection_lst = toml_array_array(connection_lst_lst, i);
        
        toml_value_t n_connections, neuron_id, n_synapses_to_neuron;
        n_connections = toml_array_int(connection_lst, 0);
        
        // check that data has been correctly loaded
        if(!n_connections.ok)
            exit(1);

        //
        (lists->synaptic_connections)[i] = malloc((n_connections.u.i * 2 + 1) * sizeof(int)); // for each connection the neuron id and the number of synapses must be stored
        (lists->synaptic_connections)[i][0] = n_connections.u.i;
    
        for(int j = 0; j<n_connections.u.i; j++){
            neuron_id = toml_array_int(connection_lst, j * 2 + 1);
            n_synapses_to_neuron = toml_array_int(connection_lst, j * 2 + 2);

            // check that the information have been correctly loaded
            if(!(neuron_id.ok && n_synapses_to_neuron.ok)){
                exit(1);
            }

            (lists->synaptic_connections)[i][j * 2 + 1] = neuron_id.u.i;
            (lists->synaptic_connections)[i][j * 2 + 2] = n_synapses_to_neuron.u.i;
        } 
    }


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
                network, delays, weights, training_zones, thresh;


    printf("Loading configuration file...\n");

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
    
    printf("General section readed\n");


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

        printf("Simulation section loaded\n");
    }
    else{ // by samples

        printf("Samples section loaded\n");
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
    else if(store_network.ok && !store_network_file.ok){
        printf("The file name to store the final network must be provided!\n");
        exit(1);
    }

    printf("Output section loaded\n");


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
    printf("Network section loaded\n");

    /*
    NETWORK SECTION
    */
    network = toml_table_string(tbl_network, "network_file");
    delays = toml_table_int(tbl_network, "delays");
    weights = toml_table_string(tbl_network, "weights");
    training_zones = toml_table_string(tbl_network, "training_zones");
    thresh = toml_table_int(tbl_network, "thresh");
    // TODO: more parameters should be added in the future

    // by default these data is not provided, only network file must be provided

    // if something is missing in configuration file, set default value
    if(!network.ok)
    {
        printf("The file to load the network must be provided\n");
        exit(1);
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
    
    // load information into config structure
    l = network.u.sl;
    conf->network_file = malloc(l * sizeof(char));
    conf->network_file = network.u.s;
    
    conf->delays_provided = delays.u.i;
    conf->weights_provided = weights.u.i;
    conf->training_zones_provided = training_zones.u.i;
    conf->thresholds_provided = thresh.u.i;

    printf("Network information correctly loaded\n");
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
    fprintf(output_file, "%f \n", results->elapsed_time_neurons_input);
    fprintf(output_file, "%f \n", results->elapsed_time_neurons_output);
    fprintf(output_file, "%f \n", results->elapsed_time_synapses);
    fclose(output_file);
}