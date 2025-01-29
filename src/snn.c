#include "../include/snn_library.h"
#include "load_data.c"
#include "helpers.c"
#include "../include/training_rules/stdp.h"

#define REFRAC_TIME
#define T 100 // tiempo total de la simulación (ms)
#define DT 1.0 // paso del tiempo (ms)
#define TC 0.9 // time constant parameter


void step_lif_neuron(spiking_nn_t *snn, int t, int neuron_identifier, unsigned char **generated_spikes){
    lif_neuron_t *lif_neuron = &(snn->lif_neurons[neuron_identifier]);
    
    float input_current = 0;

    // compute input current
    //int *i_processed_synapses = malloc(lif_neuron->input_synapse_amount * sizeof(int));
    //int i_proc_synap_amount = 0;

    for(int i=0; i<lif_neuron->input_synapse_amount; i++)
    {
        if(neuron_identifier == 3 && i==0){

        }

        int next_spike_time = lif_neuron->input_synapses[i].l_spike_times[lif_neuron->input_synapses[i].next_spike];
        if(next_spike_time == t){ // check if that must be computed in this timestep
            // Add spike time to neuron received spikes list

            input_current += lif_neuron->input_synapses[i].w;
            lif_neuron->input_synapses[i].next_spike = (lif_neuron->input_synapses[i].next_spike + 1) % lif_neuron->input_synapses[i].max_spikes; // refresh index

            // store to process STDP later
            //i_processed_synapses[i_proc_synap_amount] = i;
            //i_proc_synap_amount ++;

            // process STDP
        }
    }

    // compute membrane potential
    lif_neuron->v = lif_neuron->v * (1.0 - DT / TC) + DT / TC * lif_neuron->r * input_current;

    // generate spike if it is necessarylif_neoutput_synapsesuron
    if(lif_neuron->v >= lif_neuron->v_tresh){ // spike
        printf("Spike generated by %d at time %d\n", neuron_identifier+1, t);
        for(int i = 0; i<lif_neuron->output_synapse_amount; i++) {
            lif_neuron->output_synapses[i].l_spike_times[lif_neuron->output_synapses[i].last_spike] = t + lif_neuron->output_synapses[i].delay; // spike
            
            // will reach after delay time
            lif_neuron->output_synapses[i].last_spike++;

        }
        lif_neuron->v = lif_neuron->v_reset;

        // add spike time to synapses to compute STDP
        for(int i = 0; i<lif_neuron->output_synapse_amount; i++){
            lif_neuron->output_synapses[i].t_last_pre_spike = t;
        }
        for(int i = 0; i<lif_neuron->input_synapse_amount; i++){
            lif_neuron->input_synapses[i].t_last_post_spike = t;
        }

        generated_spikes[neuron_identifier][t] = '|';

        //STDP using
        /*for(int i = 0; i<i_proc_synap_amount; i++){
            continue;
            //lif_neuron->input_synapses[i_processed_synapses[i]].l_spike_times[next_spike-1]; // MIRAR ESTO, SI NEXT_SPIKE == 0 PROBLEMAS
            // process STDP
        } */
    }
    else{
        generated_spikes[neuron_identifier][t] = ' ';
    }
}

void forward_pass(lif_neuron_t *lif_neuron, int t, int neuron_identifier, unsigned char **generated_spikes){
    
    
    float input_current = 0;

    // compute input current
    //int *i_processed_synapses = malloc(lif_neuron->input_synapse_amount * sizeof(int));
    //int i_proc_synap_amount = 0;

    for(int i=0; i<lif_neuron->input_synapse_amount; i++)
    {
        int next_spike_time = lif_neuron->input_synapses[i].l_spike_times[lif_neuron->input_synapses[i].next_spike];
        if(next_spike_time == t){ // check if that must be computed in this timestep
            input_current += lif_neuron->input_synapses[i].w;
            lif_neuron->input_synapses[i].next_spike = (lif_neuron->input_synapses[i].next_spike + 1) % lif_neuron->input_synapses[i].max_spikes; // refresh index

            // store to process STDP later
            //i_processed_synapses[i_proc_synap_amount] = i;
            //i_proc_synap_amount ++;

            // process STDP
        }
    }

    // compute membrane potential
    lif_neuron->v = lif_neuron->v * (1.0 - DT / TC) + DT / TC * lif_neuron->r * input_current;

    // generate spike if it is necessarylif_neoutput_synapsesuron
    if(lif_neuron->v >= lif_neuron->v_tresh){ // spike
        printf("Spike generated by %d at time %d\n", neuron_identifier+1, t);
        for(int i = 0; i<lif_neuron->output_synapse_amount; i++) {
            lif_neuron->output_synapses[i].l_spike_times[lif_neuron->output_synapses[i].last_spike] = t + lif_neuron->output_synapses[i].delay; // spike
            
            // will reach after delay time
            lif_neuron->output_synapses[i].last_spike++;

        }

        lif_neuron->v = lif_neuron->v_reset;
        generated_spikes[neuron_identifier][t] = '|';

        //STDP using
        /*for(int i = 0; i<i_proc_synap_amount; i++){
            continue;
            //lif_neuron->input_synapses[i_processed_synapses[i]].l_spike_times[next_spike-1]; // MIRAR ESTO, SI NEXT_SPIKE == 0 PROBLEMAS
            // process STDP
        } */
    }
    else{
        generated_spikes[neuron_identifier][t] = ' ';
    }
}


// LIF neuron functions
void initialize_lif_neuron(lif_neuron_t *neuron, int *synapse_matrix, int excitatory_neuron, int length, 
                            int neuron_is_input, int neuron_is_output, int neuron_index){
    // Initialize lif neuron parameters
    neuron->v_tresh = 0.25;
    neuron->v=0.1;
    neuron->r = 0.9;
    neuron->excitatory = excitatory_neuron;

    neuron->n_spikes = 1; // amount of spikes that can be stored (PROVISIONALLY ONE, DEPENDS ON TYPE OF STDP????)

    //printf("Spiking neuron: %d %d\nLif neuron: %d %d\n", &p_neuron, p_neuron, &neuron, neuron);
   
    // Initialize synapses
    int input_synapse_amount = 0, output_synapse_amount = 0;
    
    // count synapses
    for(int i=0; i<length; i++){
        output_synapse_amount += synapse_matrix[neuron_index * length + i]; // row are output synapses
        input_synapse_amount += synapse_matrix[i * length + neuron_index]; // column are input synapses
    }

    // if input neuron at least one input synapse
    if(neuron_is_input==1)
        input_synapse_amount = 1;

    if(neuron_is_output==1)
        output_synapse_amount = 1; // output neurons have one output synapse for now

    // alloc memory to for synapses
    neuron->input_synapses = malloc(input_synapse_amount * sizeof(synapse_t));
    neuron->output_synapses = malloc(output_synapse_amount * sizeof(synapse_t));
    neuron->input_synapse_amount = input_synapse_amount;
    neuron->output_synapse_amount = output_synapse_amount;
}

void initialize_hh_neuron(){

}


void add_input_synapsis_to_lif_neuron(lif_neuron_t *neuron, synapse_t *synapse){
    neuron->input_synapses[neuron->actual_input_synapse] = *synapse;
    neuron->actual_input_synapse++;
}
void add_output_synapsis_to_lif_neuron(lif_neuron_t *neuron, synapse_t *synapse){
    neuron->output_synapses[neuron->actual_output_synapse] = *synapse;
    neuron->actual_output_synapse++;
}

/// @brief Initialize synapse data (wegith...)
/// @param synap 
void initialize_synapse_data(synapse_t **synap, float weight, int delay, int max_spikes, int neuron_excitatory){
    //synapse_t synap;
    (*synap)->w = weight;
    if(neuron_excitatory == 0) // if neuron is inhibitory synapse weight is negative 
        (*synap)->w = -weight;

    (*synap)->delay = delay;
    
    // initialize spike list
    (*synap)->l_spike_times = malloc(max_spikes * sizeof(int)); // 10 spikes can be stored
    for (int l = 0; l < max_spikes; l++)
        (*synap)->l_spike_times[l] = -1;
    
    // initialize control indexes
    (*synap)->next_spike = 0;
    (*synap)->last_spike = 0;
    (*synap)->max_spikes = max_spikes;

    // initialize last pre and post synaptic neurons spike times
    (*synap)->t_last_pre_spike = -1; // no spike yet
    (*synap)->t_last_post_spike = -1; // no spike yet
}

// initialize synapses of a neuron (the row)
void initialize_synapses(spiking_nn_t *snn, int pre_index, int post_index, 
                        float weight, int delay, int excitatory, int n_synapse){
    // Initialize all synapses between two neurons 
    //synapse_t synap;
    synapse_t *synap;
    synap = malloc(sizeof(synapse_t));
    initialize_synapse_data(&synap, weight, delay, 100, excitatory);

    printf("Adding synapse between %d and %d neurons\n", pre_index+1, post_index+1);
    if(snn->neuron_type==0){
        add_input_synapsis_to_lif_neuron(&(snn->lif_neurons[post_index]), synap); // synapse is input of the post synaptic neuron
        add_output_synapsis_to_lif_neuron(&(snn->lif_neurons[pre_index]), synap); // synapse is the output of the pre synaptic neuron
    }
    /*else{}*/

    // add synapse to snn synapse list
    snn->synapses[n_synapse] = synap;
}

// adds NETWORK input synapses
void add_network_input_synapse(spiking_nn_t *snn, int index, float weight, int delay){
    synapse_t *synap;
    synap = malloc(sizeof(synapse_t));
    initialize_synapse_data(&synap, weight, delay, 100, 1);

    if(snn->neuron_type==0)
        add_input_synapsis_to_lif_neuron(&(snn->lif_neurons[index]), synap);
}

void add_network_output_synapse(spiking_nn_t *snn, int index, float weight, int delay){
    synapse_t *synap;
    synap = malloc(sizeof(synapse_t));
    initialize_synapse_data(&synap, weight, delay, 100, 1);
    
    if(snn->neuron_type==0)
        add_output_synapsis_to_lif_neuron(&(snn->lif_neurons[index]), synap);
}

void initialize_network(spiking_nn_t *snn, int n_neurons, int n_input, int n_output, 
                            int *synapses, float *weights, int *delays, 
                            int n_synapses, int *neuron_excitatory){ //void *neuron_initializer()){
    // initialize neurons
    printf("Initializing network with individual neurons\n");

    int input_synapse_amount = 0, output_synapse_amount = 0;

    // check if neuron is an input neuron or output neuron
    printf("Initializing individual neurons...\n");
    for(int i = 0; i<n_neurons; i++)
    {
        // if neuron is input neuron it has only one input synapse
        int neuron_is_input = 0, neuron_is_output = 0;
        if(i<n_input) 
            neuron_is_input = 1;

        // if neuron is output neuron it has only one synapse
        if((n_neurons - i) <= n_output)
            neuron_is_output = 1;

        // initialize neurons
        if(snn->neuron_type==0)
            initialize_lif_neuron(&(snn->lif_neurons[i]), synapses, 
                        neuron_excitatory[i], n_neurons, neuron_is_input, neuron_is_output, i);
        /*
        else {

        }
        */
    }
    printf("Finished neurons initialization\n");

    printf("Initializing network synapses...\n");

    // initialize synapses reading file matrices
    int n_synapse = 0;

    for(int i = 0; i<n_neurons; i++){
        //check if neuron is input or output neuron
        int neuron_is_input = 0, neuron_is_output = 0;
        if(i<n_input) 
            neuron_is_input = 1;

        if((n_neurons - i) <= n_output)
            neuron_is_output = 1;

        for(int j = 0; j<n_neurons; j++)
            for (int z=0; z<synapses[i*n_neurons+j]; z++){
                initialize_synapses(snn, i, j, weights[n_synapse], delays[n_synapse], neuron_excitatory[i], n_synapse);
                n_synapse++;
            }

        if(neuron_is_input)
            add_network_input_synapse(snn,i,10,1);
        if(neuron_is_output)
            add_network_output_synapse(snn,i,10,1);
    }
    printf("Finished synapse initialization\n");
}


void construct_motifs(int *motif_list, int n_motifs){

}

/* main.c */
int main(int argc, char *argv[]) {
    // variables definition
    int execution_type, neuron_type, motifs, n_neurons_motifs, n_synapses;
    int *motif_list;

    int n_input, n_output; // neurons or motifs

    int *synapse_matrix, *delays_matrix, *input_delays, *output_delays;
    float *weights_matrix, *input_weights, *output_weights;
    int *neuron_excitatory;

    void *neuron_initializer; // function to initialize neurons
    void (*step)(); // functions to run a step on the simulation

    // execution type (0: clock; 1 event driven)
    execution_type = strtoul(argv[1], NULL, 10);

    // neuron type for the execution. All neurons are of the same type
    neuron_type = strtoul(argv[2], NULL, 10);

    // input are individual neurons or motifs
    motifs = strtoul(argv[3], NULL, 10);

    // print general information about the simulation / training
    printf("Execution properies:\n");
    printf("- Execution type = %d\n", execution_type);
    printf("- Neuron type for exectution = %d\n", neuron_type);
    printf("Motifs = %d\n", motifs);

    // input are motifs (motif types are loaded from a file)
    if(motifs==1) {
        load_motifs_file(argv[4], motif_list, &n_neurons_motifs);

        // move this to function?
        construct_motifs(motif_list, n_neurons_motifs);
    } 
    // input are individual neurons (only a number is necessary)
    else {
        n_neurons_motifs = strtoul(argv[4], NULL, 10);
    
        // input and output neurons
        n_input = strtoul(argv[5], NULL, 10);
        n_output = strtoul(argv[6], NULL, 10);
        
        printf("Number of neurons: %d\n", n_neurons_motifs);
        printf("Number input neurons: %d\n", n_input);
        printf("Number output neurons: %d\n", n_output);


        // move this to function?
        synapse_matrix = malloc(n_neurons_motifs * n_neurons_motifs * sizeof(int));
        neuron_excitatory = malloc(n_neurons_motifs * sizeof(int));

        // load synapsis information  
        printf("Loading synapse matrix...\n");  
        if (strcmp(argv[7], "0") == 0) { 
             // GENERATE RANDOM MATRIX
        }
        else{
            load_synapse_matrix(argv[7], synapse_matrix, &n_synapses, n_neurons_motifs);
        }
        printf("Synapse matrix loaded! Printing...\n");
        print_matrix(n_neurons_motifs, n_neurons_motifs, synapse_matrix);
        printf("Number synapses: %d\n", n_synapses);


        weights_matrix = malloc(n_synapses * sizeof(float));
        input_weights = malloc(n_input * sizeof(float)); // MORE THAN ONE SYNAPSE????
        output_weights = malloc(n_output * sizeof(float)); // ''

        delays_matrix = malloc(n_synapses * sizeof(int));
        input_delays = malloc(n_input * sizeof(int));
        output_delays = malloc(n_output * sizeof(int));


        printf("Loading weights...\n");  
        if (strcmp(argv[8], "0") == 0) { 
             // GENERATE RANDOM MATRIX
        }
        else{
            load_weight_matrix(argv[8], weights_matrix, input_weights, output_weights, n_synapses, n_input, n_output);
        }
        print_array_f(weights_matrix, n_synapses);
        printf("Weights loaded!\n");

        printf("Loading delays...\n");  
        if (strcmp(argv[9], "0") == 0) { 
             // GENERATE RANDOM MATRIX
        }
        else{
            load_delay_matrix(argv[9], delays_matrix, input_delays, output_delays, n_synapses, n_input, n_output);
        }
        print_array(delays_matrix, n_synapses);
        printf("Delays loaded!\n");

        load_excitatory_neurons(argv[10], neuron_excitatory, n_neurons_motifs);
        print_array(neuron_excitatory, n_neurons_motifs);
    }

    printf("Data load finalized, starting to generate the network.\n");

    // Initialize spiking neural network
    spiking_nn_t snn;

    // initialize neuron initialization and simulation step functions
    switch (neuron_type)
    {
        case 0: // LIF
            snn.lif_neurons = malloc(n_neurons_motifs * sizeof(lif_neuron_t));
            neuron_initializer = &initialize_lif_neuron;
            step = &step_lif_neuron;
            snn.neuron_type = 0;
            break;
        case 1: // HH
            snn.hh_neurons = malloc(n_neurons_motifs * sizeof(hh_neuron_t));
            neuron_initializer = &initialize_hh_neuron; // NOT IMPLEMENTED
            //step = &step_hh_neuron; // NOT IMPLEMENTE
            snn.neuron_type = 1;
            break;
        default:
            break;
    }
    snn.n_neurons = n_neurons_motifs;

    // reserve memory for synapses
    snn.n_synapses = n_synapses;
    snn.synapses = (synapse_t **)malloc(n_synapses * sizeof(synapse_t *));


    if(motifs==0)
        initialize_network(&snn, n_neurons_motifs, n_input, n_output, 
            synapse_matrix, weights_matrix, delays_matrix, n_synapses, neuron_excitatory);
    //else
    //    initialize_network_with_motifs();
    printf("Network succesfully initialized!\n================================\n\n\n");

    // load time steps for the simulation / training
    int time_steps = strtoul(argv[11], NULL, 10);
   
   // Check that the network has been correctly generated
    printf("Network properties:\n");
    int check_number_neurons = 0;
    for(int i = 0; i<snn.n_neurons; i++)
        check_number_neurons += 1;

    printf("Number of neurons: %d\n", check_number_neurons);

    printf("=====================\n\n");

    printf("Checking neurons...\n");
    for (int i = 0; i<snn.n_neurons; i++){
        if(snn.neuron_type==0){
            printf("Neuron n%d membrane potential: %f\n", i+1, snn.lif_neurons[i].v);
        }
    }
    printf("=================\n\n");

    printf("Checking synapses...\n");

    for (int i = 0; i<check_number_neurons; i++)
        printf("Neuron %d: input synapses = %d and output synapses = %d\n", 
                i+1, snn.lif_neurons[i].input_synapse_amount, snn.lif_neurons[i].output_synapse_amount);
    
    // Check synapses
    for(int i = 0; i<n_neurons_motifs; i++){
        printf("Neuron n%d:\n", i+1);
        printf("Checking input synapses...\n");
        for(int j = 0; j<snn.lif_neurons[i].input_synapse_amount; j++){
            //snn.lif_neurons[i].input_synapses[j].l_spike_times[10] = -1;
            printf("%f ", snn.lif_neurons[i].input_synapses[j].w);
        }
        printf("Neuron %d input synapses checked!\n", i+1);
        printf("Checking output synapses...\n");
        for(int j = 0; j<snn.lif_neurons[i].output_synapse_amount; j++){
            //snn.lif_neurons[i].output_synapses[j].l_spike_times[10] = -1;
            printf("%f ", snn.lif_neurons[i].output_synapses[j].w);
        }
        printf("Neuron %d output synapses checked!\n", i+1);
    }

    // INTRODUCIR SPIKES
    FILE *f;
    f = fopen(argv[12], "r");
    if (f == NULL){
        perror("Error opening the file\n");
        return 1;
    }
    for (int i = 0; i<n_input; i++){
        //input_trains[i] = malloc(time_steps * sizeof(int));        
        load_input_spike_train(f, &(snn.lif_neurons[i].input_synapses[0])); // input neurons
        // only have one synapse
    }  

    // print input spike trains
    printf("Printing input spike trains\n");
    for(int i = 0; i<n_input; i++){
        for (int j = 0; j<=snn.lif_neurons[i].input_synapses[0].last_spike; j++)
            printf("%d ", snn.lif_neurons[i].input_synapses[0].l_spike_times[j]);
        printf("\n");
    }
    printf("\n======================\n");

    fclose(f);
    
    
    // training 
    unsigned char **generated_spikes = malloc(n_neurons_motifs * sizeof(unsigned char *));
    for (int i = 0; i<n_neurons_motifs; i++)
        generated_spikes[i] = malloc(time_steps * sizeof(unsigned char));
    
    if(execution_type == 0){ // clock
        printf("Initializing training / simulation\n");
        int t = 0;
        while(t < time_steps){
            // procesar todas las neuronas
            #pragma omp parallel for
            for(int i=0; i<snn.n_neurons; i++){
                step(&snn, t, i, generated_spikes);

                //if(i==0 && step % 5 == 0){
                //    lif_neuron_t *lif_neuron = (lif_neuron_t *)snn.neurons[i];
                //    printf("MP: %f\n", lif_neuron->v);
                //}
            }             
            t++;

            // Compute STDP for all synapses
            for(int i = 0; i<snn.n_synapses; i++){
                float initial_w = snn.synapses[i]->w;
                printf("Synapse %d, pre_t = %d, post_t = %d\n", i, snn.synapses[i]->t_last_pre_spike, snn.synapses[i]->t_last_post_spike);
                if(snn.synapses[i]->t_last_post_spike != -1 && snn.synapses[i]->t_last_pre_spike != -1)
                    add_stdp(snn.synapses[i]);
                printf("Synapse %d weight evolution: %f -> %f\n", i, initial_w, snn.synapses[i]->w);
                snn.synapses[i]->w = 0;
            }
        }
    }
    /*else{

    }*/

    // print spikes generated by each neuron
    FILE *output_file;
    output_file = fopen("output_spikes.txt", "w");
    if(output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    for (int i = 0; i<n_neurons_motifs; i++)
    {
        for(int j = 0; j<time_steps; j++){
            fprintf(output_file, "%c", generated_spikes[i][j]);
        }
        fprintf(output_file, "\n");
    }
    
    free(snn.lif_neurons);

    return 0;
}