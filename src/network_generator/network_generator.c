#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "network_generator/network_generator.h"
#include <toml-c.h>


void generate_semi_random_synaptic_connections(network_data_t *network_data, configuration_t *conf){ // por ahora n_input_synapse y n_output_synapse no se utilizan, ya que por neurona de entrada/salida hay una unica sinapsis de entrada/salida
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;

    // amount of connection per neuron are computed
    int connections_per_neuron = ((network_data->n_synapses) - (network_data->n_input_synapses) - (network_data->n_output_synapses)) / network_data->n_neurons; // neuronas que hay entre medias en la red
    int generated_connections = 0; 

    // alocate memory
    network_data->connections[0] = (int *)malloc((network_data->n_input_neurons * 2 +1 ) * sizeof(int));
    for(i=0; i<network_data->n_neurons+1; i++){
        if(i>network_data->n_neurons - network_data->n_output_neurons)
            network_data->connections[i] = (int *)malloc(((connections_per_neuron * 2 + 2) + 1) * sizeof(int));
        else
            network_data->connections[i] = (int *)malloc(((connections_per_neuron * 2) + 1) * sizeof(int));
    }


    // generate network input synaptic connections
    int next_pos = 1; 
    network_data->connections[0][0] = network_data->n_input_neurons;
    for(i = 0; i<network_data->n_input_neurons; i++){
        int neuron_id = i;
        network_data->connections[0][next_pos] = neuron_id;
        network_data->connections[0][next_pos + 1] = 1;
        next_pos += 2;
    }

    // generate network synapses
    for(i = 1; i<network_data->n_neurons+1; i++){ 
        next_pos = 1; 
        for(j = 0; j<connections_per_neuron; j++){
            int neuron_id = rand() % network_data->n_neurons; // neuron to be connected to
            //if(neuron_id > n_neurons - connections_per_neuron + j)
            //    neuron_id = n_neurons - connections_per_neuron + j;
            network_data->connections[i][next_pos] = neuron_id;
            network_data->connections[i][next_pos + 1] = 1;
            next_pos += 2;
        }
        network_data->connections[i][0] = connections_per_neuron;
        // add output connection if neuron is output
        if(i > network_data->n_neurons - network_data->n_output_neurons){
            network_data->connections[i][next_pos] = -1;
            network_data->connections[i][next_pos + 1] = 1;
            next_pos += 2;
            
            network_data->connections[i][0] += 1;
        }
    }
}

void generate_random_synaptic_connections(network_data_t *network_data, configuration_t *conf){
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;
    int max_synapses_between_neurons = 10;
    
    int *input_synaptic_connections_per_neuron, *output_synaptic_connections_per_neuron;

    double u, lambda;//1.5;

    lambda = conf->lambda;

    // initialize synapse amount data
    network_data->n_synapses = 0; 
    network_data->n_input_synapses = 0; 
    network_data->n_output_synapses = 0;

    // pointer to store synaptic connections
    int *temp_synaptic_connections = (int *)malloc((1 + 2 * network_data->n_neurons + 2) * sizeof(int)); // 1 is the number of synaptic connections, for each neuron 2 numbers (worst case) and another two if neuron is network output
    int number_neuron_connections=0; // variable to store to how much neurons is conected a neurons (not synaptic connections)
    int synapses; // generated synapse amount from neuron i to j
    int next_pos = 1; 

    input_synaptic_connections_per_neuron = (int *)malloc(network_data->n_neurons * sizeof(int));
    output_synaptic_connections_per_neuron = (int *)malloc(network_data->n_neurons * sizeof(int));
    // initialize lists
    for(i = 0; i<network_data->n_neurons; i++){
        input_synaptic_connections_per_neuron[i] = 0;
        output_synaptic_connections_per_neuron[i] = 0;
    }

    // generate network input synaptic connections
    for(i = 0; i<network_data->n_input_neurons; i++){ 
        u = (double)rand() / RAND_MAX;
        synapses = (int)(-log(1-u) / lambda) / 2;

        // 
        if(synapses == 0){
            synapses = 1; // input neurons must have at least one input synapse
        }
        synapses = 1;
        temp_synaptic_connections[next_pos] = i; // network input synapse to neuron i
        temp_synaptic_connections[next_pos+1] = synapses; // amount of synapses 
        next_pos += 2;
        number_neuron_connections +=1; // total input synapses
        
        // add synapse amount to total synapse number and to input synapse number
        network_data->n_input_synapses += synapses;
        network_data->n_synapses += synapses;

        // count synaptic connections per neuron
        input_synaptic_connections_per_neuron[i] += synapses; // TODO: in the future can be more than one
    }
    temp_synaptic_connections[0] = number_neuron_connections;

    // allocate memory for complete synaptic connections variable
    network_data->connections[0] = (int *)malloc((temp_synaptic_connections[0] * 2 + 1) * sizeof(int));

    // write on file (less memory) and copy to general
    for(i = 0; i<(temp_synaptic_connections[0] * 2 + 1); i++)
        network_data->connections[0][i] = temp_synaptic_connections[i];


    // generate synaptic connections
    for(i=1; i<(network_data->n_neurons+1); i++){ // network input synapses not taken into account
        next_pos = 1;
        number_neuron_connections = 0;

        // for each neuron, generate synapses
        for(j=0; j<(network_data->n_neurons); j++){ // j == n_neurons --> network output synapses
            u = (double)rand() / RAND_MAX;
            synapses = (int)(-log(1-u) / lambda) / 2;

            if(synapses>0){
                // check if synapses amount is bigger than maximum
                if(synapses > conf->max_connections_pair_neurons)
                    synapses = conf->max_connections_pair_neurons;

                temp_synaptic_connections[next_pos] = j; // network input synapse to neuron j
                temp_synaptic_connections[next_pos+1] = synapses; // amount of synapses 
                next_pos += 2;
                number_neuron_connections +=1; // neuron is connected to one more neuron

                network_data->n_synapses += synapses;

                // count number of synaptic connections per neuron
                input_synaptic_connections_per_neuron[j] += synapses; // neuron j input synapse
                output_synaptic_connections_per_neuron[i] += synapses; // neuron i output synapse
            }            
        }

        // if neuron is output neuron
        if(i > network_data->n_neurons - network_data->n_output_neurons){
            temp_synaptic_connections[next_pos] = -1; // network input synapse to neuron i
            temp_synaptic_connections[next_pos+1] = 1; // for now only one synapse as output
            next_pos += 2;
            number_neuron_connections += 1; // neuron is connected to one more neuron
            
            // add synpase amount to output synapses and total synapses
            network_data->n_output_synapses += 1; 
            network_data->n_synapses += 1;

            // count output synapses
            output_synaptic_connections_per_neuron[i] += 1; // TODO: in the future can be more than one
        }


        temp_synaptic_connections[0] = number_neuron_connections;

        // allocate memory for complete synaptic connections variable
        network_data->connections[i] = (int *)malloc((temp_synaptic_connections[0] * 2 + 1) * sizeof(int));

        // write on file (less memory) and copy to general
        for(j = 0; j<(temp_synaptic_connections[0] * 2 + 1); j++)
            network_data->connections[i][j] = temp_synaptic_connections[j];
    }

    // copy number of input and output synapses
    for(i=0; i<network_data->n_neurons; i++){
        network_data->n_input_synapses_per_neuron[i] = input_synaptic_connections_per_neuron[i];
        network_data->n_output_synapses_per_neuron[i] = output_synaptic_connections_per_neuron[i];
    }
}


void generate_random_synaptic_weights(network_data_t *network_data, configuration_t *conf){
    int i,j, l;

    int next_synapse = 0, valid;
    double w, min_w, max_w;

    switch(conf->weight){
        case 0: // default
            min_w = -99999999;
            max_w = 9999999;
            break;
        case 1: // interval
            min_w = conf->min_weight;
            max_w = conf->max_weight;
            break;
        case 2: // no limit
            min_w = -99999999;
            max_w = 9999999;
            break;    
    }

    // all network input synapses have positive weights
    for(i=1; i<(network_data->connections[0][0] * 2 + 1); i+=2){
        for(j = 0; j<(network_data->connections[0][i+1]); j++){
            valid = 0;
            w = 0;
            
            // if weight == 0 or 2, default 
            if(conf->weight == 0 || conf->weight == 2){
                while(valid == 0){
                    w = ((double)rand() / (double)(RAND_MAX)) * 5;
                    if(w>0.005) valid = 1;
                }
            }
            // use interval
            else{
                w = (double)(rand() % (int)(max_w));
                
                if(w > (double)max_w)
                    w = (double)max_w;
                if(w < (double)min_w)
                    w = (double)min_w;

                // check if we are using integers
                if(conf->integers == 1){
                    int temp_w = (int)w;
                    w = (double)temp_w; // convert to int and then to float
                }
                else{
                    w = w * 0.99;
                }
            }
                //printf("generated w %f\n", w);

            // store weight
            network_data->weights[next_synapse] = (double)w; 
            next_synapse ++;
        }
    }

    // rest of synapses
    for(i=1; i<(network_data->n_neurons + 1); i++){
        for(j=1; j<(network_data->connections[i][0] * 2 +1); j+=2){
            for(l = 0; l<network_data->connections[i][j+1]; l++){
                // generate random value
                int valid = 0;
                double w = 0;
                
                // if weight == 0 or 2, default 
                if(conf->weight == 0 || conf->weight == 2){
                    while(valid == 0){
                        w = ((double)rand() / (double)(RAND_MAX)) * 5;
                        if(w>0.005) valid = 1;
                    }
                }
                // use interval
                else{
                    w = (double)(rand() % (int)(max_w));

                    if(w > (double)max_w)
                        w = (double)max_w;
                    if(w < (double)min_w)
                        w = (double)min_w;

                    // check if we are using integers
                    if(conf->integers == 1){
                        int temp_w = (int)w;
                        w = (double)temp_w; // convert to int and then to float
                    }
                    else{
                        w = w * 0.99;
                    }
                }                
                
                /*while(valid == 0){
                    w = ((float)rand() / (float)(RAND_MAX)) * 5;
                    if(w>0.005) valid = 1;
                }*/


                //printf("generated w %f\n", w);
                if(j == network_data->n_neurons){ // network output synapse: always positive
                    network_data->weights[next_synapse] = (double)w; 
                }
                else{ // other synapses can be positive or negative depending on neuron
                    if(network_data->behaviour_list[i-1] == 1){ // excitatory neuron
                        network_data->weights[next_synapse] = (double)w; 
                    }
                    else{ // inhibitory neuron
                        network_data->weights[next_synapse] = (double)-w;
                    }
                }
                next_synapse ++;
            }
        }
    }
}

void generate_random_synaptic_delays(network_data_t *network_data, configuration_t *conf){//int n_neurons, int **synaptic_connections){
    int i,j, l;
    int next_synapse = 0, min_delay, max_delay;
    double u, lambda;

    lambda = conf->lambda_delays;

    switch(conf->latency){
        case 0: // default
            min_delay = 1;
            max_delay = 1;
            break;
        case 1: // interval
            min_delay = conf->min_latency;
            max_delay = conf->max_latency;
            break;
        case 2: // no limit
            min_delay = 1;
            max_delay = 999999; 
            break;
    }

    //printf("Lantecy value: %d, min: %d, max: %d\n", conf->latency, min_delay, max_delay);
    for(next_synapse = 0; next_synapse<network_data->n_input_synapses; next_synapse++)
        network_data->latency_list[next_synapse] = 1;

    
    for(next_synapse = network_data->n_input_synapses; next_synapse<network_data->n_synapses; next_synapse++){
        network_data->latency_list[next_synapse] = 1;
        u = (double)rand() / RAND_MAX;
        network_data->latency_list[next_synapse] = (int)(-log(1-u) / lambda); //% max_delay;

        // check if it is in the interval
        if(network_data->latency_list[next_synapse] < min_delay)
            network_data->latency_list[next_synapse] = min_delay;

        if(network_data->latency_list[next_synapse] > max_delay)
            network_data->latency_list[next_synapse] = max_delay;

        // latency can not be 0
        if(network_data->latency_list[next_synapse] == 0) 
            network_data->latency_list[next_synapse] = 1; // delay can not be 0

        //printf("%d ", network_data->latency_list[next_synapse]);
    }
}

void generate_random_training_type(network_data_t *network_data, configuration_t *conf){
    int i,j;

    int next_synapse = 0;
    int learning_rules = 3;

    for(i = 0; i<network_data->n_synapses; i++){
        network_data->training_zones_list[i] = rand() % learning_rules;
    }
}

// THIS FUNCTION MUST BE REVISED FOR NEGATIVEVALUES
void generate_neurons_data(network_data_t *network_data, configuration_t *conf){
    int i, random_value;

    // loop over all neurons
    for(i = 0; i<network_data->n_neurons; i++){

        // behaviour TODO: THIS SOULD BE CHANGED IF MORE OPTIONS ARE ADDED IN THE FUTURE; NOW IT DOES NOT MAKE SENSE
        if(conf->behaviour == 0){
            network_data->behaviour_list[i] = 1; // default (1 is excitatory?????) TODO
        }
        else{
            random_value = rand() % 100;
            if(random_value > 75){
                network_data->behaviour_list[i] = 0;
            }
            else{
                network_data->behaviour_list[i] = 1;
            }
        }

        // v_thres
        if(conf->v_thres == 0){ // default
            network_data->v_thres_list[i] = 150;
        }
        else if(conf->v_thres == 1){ // between min and max
            int max_value = (int)(conf->max_v_thres);
            int min_value = (int)(conf->min_v_thres);

            random_value = rand() % max_value;
            if(random_value < min_value) 
                random_value = min_value;
            network_data->v_thres_list[i] = (double)random_value;
        }
        else{ // random --> NOT SENSE
            // TODO
        }

        // v_rest
       if(conf->v_rest == 0){ // default
            network_data->v_rest_list[i] = 0;
        }
        else if(conf->v_rest == 1){ // between min and max
            int max_value = (int)(conf->max_v_rest);
            int min_value = (int)(conf->min_v_rest);

            random_value = rand() % max_value;
            if(random_value < min_value) 
                random_value = min_value;
            network_data->v_rest_list[i] = (double)random_value;
        }
        else{ // random --> NOT SENSE
            // TODO
        }

        // R
        if(conf->R == 0){ // default
            network_data->r_list[i] = 10;
        }
        else if(conf->R == 1){ // between min and max
            random_value = rand() % conf->max_R;
            if(random_value < conf->min_R) 
                random_value = conf->min_R;
            network_data->r_list[i] = random_value;
        }
        else{ // random --> NOT SENSE
            // TODO
        }

        // refract. time
        if(conf->refract_time == 0){ // default
            network_data->refract_time_list[i] = 3;
        }
        else if(conf->refract_time == 1){ // between min and max
            random_value = rand() % conf->max_refract_time;
            if(random_value < conf->min_refract_time) 
                random_value = conf->min_refract_time;
            network_data->refract_time_list[i] = random_value;
        }
        else{ // random --> NOT SENSE
            // TODO
        }
    }
}


void read_configuration_file(char *file_name, network_data_t *network_data, configuration_t *conf){
    FILE *f = NULL;
    char errbuf[100];
    int i, l;
    int l_file_names = 300;

    // define table and variables to store configuration parameters
    toml_table_t *tbl, *tbl_general, *tbl_neurons, *tbl_synapses;
    
    toml_value_t neurons, synapses, n_neurons, n_input_neurons, n_output_neurons, n_synapses, n_input_synapses, n_output_synapses, output_file_name, lambda, lambda_delays, 
                max_connections_pair_neurons, integers, behaviour, min_behaviour, max_behaviour, R, min_R, max_R, refract_time, min_refract_time, max_refract_time, 
                v_thres, min_v_thres, max_v_thres, v_rest, min_v_rest, max_v_rest,
                latency, min_latency, max_latency, training_zone, min_training_zone, max_training_zone, weight, min_weight, max_weight,
                output_is_separated;


    /* open TOML file */
    f = fopen(file_name, "r"); // append mode, no overwriting
    if (f == NULL){
        perror("Error opening the file\n");
        exit(1);
    }    
    
    /* read TOML file */
    tbl = toml_parse_file(f, errbuf, l_file_names);

    // get sections from file
    tbl_general = toml_table_table(tbl, "general");
    tbl_neurons = toml_table_table(tbl, "neurons");
    tbl_synapses = toml_table_table(tbl, "synapses");

    /*
    GENERAL SECTION
    */
    neurons = toml_table_int(tbl_general, "neurons");
    synapses = toml_table_int(tbl_general, "synapses");

    n_neurons = toml_table_int(tbl_general, "n_neurons");
    n_input_neurons = toml_table_int(tbl_general, "n_input_neurons");
    n_output_neurons = toml_table_int(tbl_general, "n_output_neurons");
    n_synapses = toml_table_int(tbl_general, "n_synapses");
    n_input_synapses = toml_table_int(tbl_general, "n_input_synapses");
    n_output_synapses = toml_table_int(tbl_general, "n_output_synapses");

    output_file_name = toml_table_string(tbl_general, "output_file");
    output_is_separated = toml_table_int(tbl_general, "output_is_separated");

    lambda = toml_table_double(tbl_general, "lambda");
    lambda_delays = toml_table_double(tbl_general, "lambda_delays");
    integers = toml_table_int(tbl_general, "integers");
    max_connections_pair_neurons = toml_table_int(tbl_general, "max_connections_pair_neurons");

    // TODO: Manage neurons and synapses
    
    // if something is missing in configuration file, set default value
    if(!(n_neurons.ok && n_input_neurons.ok && n_output_neurons.ok)){
        printf("Number of neurons, input neurons and output neurons must be provided!\n"); // clock-based
    }

    if(synapses.ok && synapses.u.i == 1){
        if(!(n_synapses.ok && n_input_synapses.ok && n_output_synapses.ok)){
            printf("Number of synapses, input synapses and output synapses must be provided!\n"); // clock-based
        }        
    }

    if(!output_file_name.ok){
        printf("File to store the network must be provided!\n");
        exit(1);
    }

    if(!output_is_separated.ok){
        printf("It is not mentioned if output must be stored in different files. By default yes.\n");
        output_is_separated.u.i = 1;
    }

    if(!lambda.ok){
        printf("Lambda value must be provided!\n");
        exit(1);
    }

    if(!lambda_delays.ok){
        printf("Lambda value for delays must be provided!\n");
        exit(1);
    }

    if(!integers.ok){
        printf("No value provided to indicate if values are integers, using float by default\n");
        integers.u.i = 0;
    }

    if(!max_connections_pair_neurons.ok){
        max_connections_pair_neurons.u.i = 9999999; // not maximum set
    }

    // load information into config structure
    network_data->n_neurons = n_neurons.u.i;
    network_data->n_input_neurons = n_input_neurons.u.i;
    network_data->n_output_neurons = n_output_neurons.u.i;
    network_data->n_synapses = n_synapses.u.i;
    network_data->n_input_synapses = n_input_synapses.u.i;
    network_data->n_output_synapses = n_output_synapses.u.i;

    conf->output_file_name = output_file_name.u.s;
    conf->output_is_separated = output_is_separated.u.i;

    conf->lambda = lambda.u.d;
    conf->lambda_delays = lambda_delays.u.d;
    conf->integers = integers.u.i; // if 0, no integers, else, weights and membrane potentials are integers
    conf->max_connections_pair_neurons = max_connections_pair_neurons.u.i;

    // neurons section
    v_thres = toml_table_int(tbl_neurons, "v_thres");
    min_v_thres = toml_table_double(tbl_neurons, "v_thres_min");
    max_v_thres = toml_table_double(tbl_neurons, "v_thres_max");

    v_rest = toml_table_int(tbl_neurons, "v_rest");
    min_v_rest = toml_table_double(tbl_neurons, "v_rest_min");
    max_v_rest = toml_table_double(tbl_neurons, "v_rest_max");
    
    R = toml_table_int(tbl_neurons, "R");
    min_R = toml_table_int(tbl_neurons, "R_min");
    max_R = toml_table_int(tbl_neurons, "R_max");
    
    refract_time = toml_table_int(tbl_neurons, "refract_time");
    min_refract_time = toml_table_int(tbl_neurons, "refract_time_min");
    max_refract_time = toml_table_int(tbl_neurons, "refract_time_max");
    
    behaviour = toml_table_int(tbl_neurons, "behaviour");
    min_behaviour = toml_table_int(tbl_neurons, "behaviour_min");
    max_behaviour = toml_table_int(tbl_neurons, "behaviour_max");
    
    latency = toml_table_int(tbl_synapses, "latency");
    min_latency = toml_table_int(tbl_synapses, "latency_min");
    max_latency = toml_table_int(tbl_synapses, "latency_max");
    
    training_zone = toml_table_int(tbl_synapses, "training_zone");
    min_training_zone = toml_table_int(tbl_synapses, "training_zone_min");
    max_training_zone = toml_table_int(tbl_synapses, "training_zone_max");
    
    weight = toml_table_int(tbl_synapses, "weight");
    min_weight = toml_table_double(tbl_synapses, "weight_min");
    max_weight = toml_table_double(tbl_synapses, "weight_max");
    
    // check what parameters are provided to use as intervals in randomization, otherwise
    // if 0, default values are used, if 1, random values inside provided interval, else random values
    if(!v_thres.ok){
        conf->v_thres = 0;
    }
    else{
        conf->v_thres = v_thres.u.i;
    }

    if(!behaviour.ok){
        conf->behaviour = 0;
    }
    else{
        conf->behaviour = behaviour.u.i;
    }

    if(!v_rest.ok){
        conf->v_rest = 0;
    }
    else{
        conf->v_rest = v_rest.u.i;
    }

    if(!R.ok){
        conf->R = 0;
    }
    else{
        conf->R = R.u.i;
    }

    if(!refract_time.ok){
        conf->refract_time = 0;
    }
    else{
        conf->refract_time = refract_time.u.i;
    }

    if(!latency.ok){
        conf->latency = 0;
    }
    else{
        conf->latency = latency.u.i;
    }

    if(!training_zone.ok){
        conf->training_zone = 0;
    }
    else{
        conf->training_zone = training_zone.u.i;
    }

    if(!weight.ok){
        conf->weight = 0;
    }
    else{
        conf->weight = weight.u.i;
    }

    // if values are provided (value 1), read
    if(v_thres.ok && v_thres.u.i==1){
        if(!(min_v_thres.ok && max_v_thres.ok)){
            printf("Min and max values for threshold must be provided!\n");
        }

        conf->min_v_thres = min_v_thres.u.d;
        conf->max_v_thres = max_v_thres.u.d;
    }

    if(v_rest.ok && v_rest.u.i==1){
        if(!(min_v_rest.ok && max_v_rest.ok)){
            printf("Min and max values for resting membrane potential must be provided!\n");
        }

        conf->min_v_rest = min_v_rest.u.d;
        conf->max_v_rest = max_v_rest.u.d;
    }

    if(behaviour.ok && behaviour.u.i==1){
        if(!(min_v_rest.ok && max_v_rest.ok)){
            printf("Min and max values for neuron behaviour must be provided!\n");
        }

        conf->min_behaviour = min_behaviour.u.i;
        conf->max_behaviour = max_behaviour.u.i;
    }

    if(R.ok && R.u.i==1){
        if(!(min_R.ok && max_R.ok)){
            printf("Min and max values for neurons resistance must be provided!\n");
        }

        conf->min_R = min_R.u.i;
        conf->max_R = max_R.u.i;
    }

    if(refract_time.ok && refract_time.u.i==1){
        if(!(min_refract_time.ok && max_refract_time.ok)){
            printf("Min and max values for neurons refract. time must be provided!\n");
        }

        conf->min_refract_time = min_refract_time.u.i;
        conf->max_refract_time = max_refract_time.u.i;
    }

    if(latency.ok && latency.u.i==1){
        if(!(min_latency.ok && max_latency.ok)){
            printf("Min and max values for synapses delays must be provided!\n");
        }

        conf->min_latency = min_latency.u.i;
        conf->max_latency = max_latency.u.i;
    }

    if(training_zone.ok && training_zone.u.i==1){
        if(!(min_training_zone.ok && max_training_zone.ok)){
            printf("Min and max values for synapses training zones must be provided!\n");
        }

        conf->min_training_zone = min_training_zone.u.i;
        conf->max_training_zone = max_training_zone.u.i;
    }

    if(weight.ok && weight.u.i==1){
        if(!(min_weight.ok && max_weight.ok)){
            printf("Min and max values for synapses weights must be provided!\n");
        }

        conf->min_weight = min_weight.u.d;
        conf->max_weight = max_weight.u.d;
    }
}

void store_network_in_toml_format(char *file_name, network_data_t *network_data, configuration_t *conf){
    FILE *f;
    int i;
    char *original_file_name, *file_name_neurons, *file_name_synapses;
    FILE *f_neurons, *f_synapses;


    f = fopen(file_name, "w"); // write
    if (f == NULL){
        perror("Error opening the file\n");
        exit(1);
    }    
    
    // write general information
    printf("Storing general information...\n");

    fprintf(f, "[general]\n");
    fprintf(f, "    neurons = %d\n",network_data->n_neurons);
    fprintf(f, "    input_neurons = %d\n", network_data->n_input_neurons);
    fprintf(f, "    output_neurons = %d\n", network_data->n_output_neurons);
    fprintf(f, "    synapsis = %d\n", network_data->n_synapses);
    fprintf(f, "    input_synapsis = %d\n", network_data->n_input_synapses);
    fprintf(f, "    output_synapsis = %d\n", network_data->n_output_synapses);
    fprintf(f, "    network_is_separated = %d\n\n", conf->output_is_separated);

    printf("General information stored!\n");

    /*
    If output is separaed is 1, then all lists are stored in apart files with the name of the
    original file and an extra indicating what parameter is stored into that file. Else, all
    is stored in only one file. 
    */

    original_file_name = strtok(file_name, "."); // split file name by extension
    int len = strlen(original_file_name);

    file_name_neurons = malloc((len + 20) * sizeof(char));
    file_name_synapses = malloc((len + 25) * sizeof(char));

    strcpy(file_name_neurons, original_file_name);  // copy original file name
    strcpy(file_name_synapses, original_file_name);
    
    strcat(file_name_neurons, "_neurons.toml"); // add extension to original file name for neurons and synapses
    strcat(file_name_synapses, "_synapses.toml");

    printf("%s\n", file_name_neurons);
    printf("%s\n", file_name_synapses);

    // open files if information must be stored separated
    if(conf->output_is_separated == 1){
        f_neurons = fopen(file_name_neurons, "w"); // write
        if (f_neurons == NULL){
            perror("Error opening the file\n");
            exit(1);
        }  

        f_synapses = fopen(file_name_synapses, "w"); // write
        if (f_synapses == NULL){
            perror("Error opening the file\n");
            exit(1);
        }  
    }

    // write neurons information
    printf("Storing neurons information...\n");

    fprintf(f, "[neurons]\n");
    fprintf(f, "    behaviour = 1\n"); // TODO: THIS IS TEMPORAL

    if(conf->output_is_separated != 1){
        fprintf(f, "    behaviour_list = [");
        for(i=0; i<network_data->n_neurons-1; i++){
            fprintf(f, "%d, ", network_data->behaviour_list[i]);
        }
        fprintf(f, "%d]\n", network_data->behaviour_list[network_data->n_neurons-1]);
    }

    fprintf(f, "    v_thres = 1\n"); // TODO: THIS IS TEMPORAL
    if(conf->output_is_separated != 1){
        fprintf(f, "    v_thres_list = [");
            for(i=0; i<network_data->n_neurons-1; i++){
                fprintf(f, "%f, ", network_data->v_thres_list[i]);
            }
        fprintf(f, "%f]\n", network_data->v_thres_list[network_data->n_neurons-1]);
    }

    fprintf(f, "    v_rest = 1\n"); // TODO: THIS IS TEMPORAL
    if(conf->output_is_separated != 1){
        fprintf(f, "    v_rest_list = [");
        for(i=0; i<network_data->n_neurons-1; i++){
            fprintf(f, "%f, ", network_data->v_rest_list[i]);
        }
        fprintf(f, "%f]\n", network_data->v_rest_list[network_data->n_neurons-1]);
    }

    fprintf(f, "    t_refract = 1\n"); // TODO: THIS IS TEMPORAL
    if(conf->output_is_separated != 1){
        fprintf(f, "    t_refract_list = [");
        for(i=0; i<network_data->n_neurons-1; i++){
            fprintf(f, "%d, ", network_data->refract_time_list[i]);
        }
        fprintf(f, "%d]\n", network_data->refract_time_list[network_data->n_neurons-1]);
    }

    if(conf->output_is_separated != 1){
        fprintf(f, "    input_synapsis = [");
        for(i=0; i<network_data->n_neurons-1; i++){
            fprintf(f, "%d, ", network_data->n_input_synapses_per_neuron[i]);
        }
        fprintf(f, "%d]\n", network_data->n_input_synapses_per_neuron[network_data->n_neurons-1]);
    }
    
    if(conf->output_is_separated != 1){
        fprintf(f, "    output_synapsis = [");
        for(i=0; i<network_data->n_neurons-1; i++){
            fprintf(f, "%d, ", network_data->n_output_synapses_per_neuron[i]);
        }
        fprintf(f, "%d]\n", network_data->n_output_synapses_per_neuron[network_data->n_neurons-1]);
    }


    fprintf(f, "\n");

    // if is separated == 1, store neurons information in plain text
    if(conf->output_is_separated == 1){
        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%d ", network_data->behaviour_list[i]);
        }
        fprintf(f_neurons, "\n");

        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%f ", network_data->v_thres_list[i]);
        }
        fprintf(f_neurons, "\n");

        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%f ", network_data->v_rest_list[i]);
        }
        fprintf(f_neurons, "\n");

        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%d ", network_data->refract_time_list[i]);
        }
        fprintf(f_neurons, "\n");

        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%d ", network_data->n_input_synapses_per_neuron[i]);
        }
        fprintf(f_neurons, "\n");

        for(i=0; i<network_data->n_neurons; i++){
            fprintf(f_neurons, "%d ", network_data->n_output_synapses_per_neuron[i]);
        }
        fprintf(f_neurons, "\n");
    }


    printf("Neurons information stored!\n");


    // write synapses information
    printf("Storing synapses information...\n");
    
    fprintf(f, "[synapsis]\n");

    fprintf(f, "    latency = 1\n"); // TODO: THIS IS TEMPORAL
    
    if(conf->output_is_separated != 1){
        fprintf(f, "    latency_list = [");
        for(i=0; i<network_data->n_synapses-1; i++){
            fprintf(f, "%d, ", network_data->latency_list[i]);
        }
        fprintf(f, "%d]\n", network_data->latency_list[network_data->n_synapses-1]);
    }

    if(conf->output_is_separated != 1){
        fprintf(f, "    weights = [");
        for(i=0; i<network_data->n_synapses-1; i++){
            fprintf(f, "%f, ", network_data->weights[i]);
        }
        fprintf(f, "%f]\n", network_data->weights[network_data->n_synapses-1]);
    }

    fprintf(f, "    training_zones = 0\n"); // TODO: THIS IS TEMPORAL
    if(conf->output_is_separated != 1){
        fprintf(f, "    training_zones_list = [");
        for(i=0; i<network_data->n_synapses-1; i++){
            fprintf(f, "%d, ", network_data->training_zones_list[i]);
        }
        fprintf(f, "%d]\n", network_data->training_zones_list[network_data->n_synapses-1]);
    }

    if(conf->output_is_separated != 1){
        fprintf(f, "    connections = [");
        for(int i=0; i<(network_data->n_neurons+1); i++){
            fprintf(f, "[");
            for(int j=0; j<(network_data->connections[i][0] * 2); j++)
                fprintf(f, "%d, ", network_data->connections[i][j]);

            if(i < network_data->n_neurons)
                fprintf(f, "%d], ", network_data->connections[i][network_data->connections[i][0] * 2]);
            else
                fprintf(f, "%d]", network_data->connections[i][network_data->connections[i][0]* 2]);
        }
        fprintf(f, "]\n");
    }
    printf("Synapses information stored!\n");

    // if is separated == 1, store neurons information in plain text
    if(conf->output_is_separated == 1){
        for(i=0; i<network_data->n_synapses; i++){
            fprintf(f_synapses, "%d ", network_data->latency_list[i]);
        }
        fprintf(f_synapses, "\n");

        for(i=0; i<network_data->n_synapses; i++){
            fprintf(f_synapses, "%f ", network_data->weights[i]);
        }
        fprintf(f_synapses, "\n");

        for(i=0; i<network_data->n_synapses; i++){
            fprintf(f_synapses, "%d ", network_data->training_zones_list[i]);
        }
        fprintf(f_synapses, "\n\n");

        for(int i=0; i<(network_data->n_neurons+1); i++){
            for(int j=0; j<(network_data->connections[i][0] * 2 + 1); j++)
                fprintf(f_synapses, "%d ", network_data->connections[i][j]);
            fprintf(f_synapses, "\n");
        }
    }

    fclose(f);
}