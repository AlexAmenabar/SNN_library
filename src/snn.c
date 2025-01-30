#include "../include/snn_library.h"
#include "../include/load_data.h"
#include "helpers.c"
//#include "../include/training_rules/stdp.h"

#define REFRAC_TIME
#define T 100 // tiempo total de la simulación (ms)
#define DT 1.0 // paso del tiempo (ms)
#define TC 0.9 // time constant parameter

/** LIF NEURON FUNCTIONS */

void initialize_lif_neuron(spiking_nn_t *snn, int neuron_index, int excitatory, int *synapse_matrix){
    // Initialize lif neuron parameters
    int i;

    lif_neuron_t *neuron = &(snn->lif_neurons[neuron_index]);
    neuron->v_tresh = 0.25;
    neuron->v=0.1;
    neuron->r = 0.9;
    neuron->excitatory = excitatory;

    // check if neuron is input/output neuron 
    if(neuron_index < snn->n_input)
        neuron->is_input_neuron = 1;
    else
        neuron->is_input_neuron = 0;

    if((snn->n_neurons - snn->n_output) <= neuron_index)
        neuron->is_output_neuron = 1;
    else
        neuron->is_output_neuron = 0;

    // Initialize synapse data
    int n_input_synapse = 0, n_output_synapse = 0;
    
    // count input synapses
    for(i=0; i<snn->n_neurons; i++)
        n_input_synapse += synapse_matrix[i * (snn->n_neurons+1) + neuron_index]; // input synapses are found on the column
   
    // count output synapses
    for(i=1; i<snn->n_neurons+1;i++)
        n_output_synapse += synapse_matrix[(neuron_index + 1) * (snn->n_neurons + 1) + i]; // input synapses are found on the row

    // reserve memory to store synapses indexes
    neuron->input_synapse_indexes = malloc(n_input_synapse * sizeof(int));
    neuron->output_synapse_indexes = malloc(n_output_synapse * sizeof(int));
    neuron->n_input_synapse = n_input_synapse;
    neuron->n_output_synapse = n_output_synapse;

    neuron->next_input_synapse = 0; 
    neuron->next_output_synapse = 0;
}

void initialize_network_neurons(spiking_nn_t *snn, int *neuron_behaviour_list, int *synapse_matrix){
    int i;

    if(snn->neuron_type == 0){ // lif neurons
        snn->lif_neurons = (lif_neuron_t *)malloc(snn->n_neurons * sizeof(lif_neuron_t)); // reserve memory for lif neurons
    }
    // else {}

    for(i=0; i<snn->n_neurons; i++)
        if(snn->neuron_type == 0) // lif neuron
            initialize_lif_neuron(snn, i, neuron_behaviour_list[i], synapse_matrix);
}

/** SYNAPSE FUNCTIONS */

void initialize_synapse(synapse_t *synapse, float w, int delay, int training){
    synapse->w = w;
    synapse->delay = delay;
    synapse->max_spikes = MAX_SPIKES;
    synapse->t_last_post_spike = -1;
    synapse->t_last_pre_spike = -1;
    synapse->post_neuron_computed = -1;
    synapse->pre_neuron_computed = -1;
    synapse->last_spike = 0; 
    synapse->next_spike = 0;

    synapse->l_spike_times = (int *)malloc(MAX_SPIKES * sizeof(int));
    for(int i = 0; i<MAX_SPIKES; i++)
        synapse->l_spike_times[i] = -1; // no spikes yet

    //TODO --> training
}

void initialize_network_synapses(spiking_nn_t *snn, int n_synapses, float *weight_list, int *delay_list, int *training_zones){
    // reserve memory for synapses
    snn->synapses = (synapse_t *)malloc(n_synapses * sizeof(synapse_t));

    for(int i = 0; i<n_synapses; i++)
        initialize_synapse(&(snn->synapses[i]), weight_list[i], delay_list[i], training_zones[i]);
}


void add_input_synapse_to_lif_neuron(lif_neuron_t *neuron, int synapse_index){
    neuron->input_synapse_indexes[neuron->next_input_synapse] = synapse_index;
    neuron->next_input_synapse += 1;
}

void add_input_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index){
    if(snn->neuron_type == 0)
        add_input_synapse_to_lif_neuron(&snn->lif_neurons[neuron_index], synapse_index);
    // else{}
}

void add_output_synapse_to_lif_neuron(lif_neuron_t *neuron, int synapse_index){
    neuron->output_synapse_indexes[neuron->next_output_synapse] = synapse_index;
    neuron->next_output_synapse += 1;
}

void add_output_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index){
    if(snn->neuron_type == 0)
        add_output_synapse_to_lif_neuron(&snn->lif_neurons[neuron_index], synapse_index);
    // else{}
}

void connect_neurons_and_synapses(spiking_nn_t *snn, int *synapse_matrix){
    int n_neurons = snn->n_neurons;
    int n_input = snn->n_input;
    int n_output = snn->n_output;

    int i, j, l, n_synapse;

    // connect neurons with input synapses
    
    for(i = 0; i<(n_neurons + 1); i++){
        for(j = 0; j<(n_neurons + 1); j++){
            for(l = 0; synapse_matrix[i * (n_neurons+1) + j]){
                if(i<n_input){ // only input synapses for input neurons
                    // add input
                }
                else if(i>=(n_neurons-n_output)){ // only output for output neurons

                }
                else{ // normal
                    
                }
            }
        }
    }
}


void initialize_network(spiking_nn_t *snn, int neuron_type, int n_neurons, int n_input, int n_output, int n_synapses, int *neuron_behaviour_list,
                            int *synapse_matrix, float *weight_list, int *delay_list, int *training_zones){ //void *neuron_initializer()){
#ifdef DEBUF
    printf("Initializing network...\n");
#endif

    // initialize general information
    snn->n_neurons = n_neurons;
    snn->n_synapses = n_synapses;
    snn->n_input = n_input;
    snn->n_output = n_output;
    snn->neuron_type = neuron_type;

    // initialize neurons of the network
    initialize_network_neurons(snn, neuron_behaviour_list, synapse_matrix);

    // initialize synapses
    initialize_network_synapses(snn, n_synapses, weight_list, delay_list, training_zones);

    // connect neurons and synapses
    connect_neurons_and_synapses(snn, synapse_matrix);
}



/* main.c */
int main(int argc, char *argv[]) {
    // variables definition
    int execution_type, neuron_type, motifs, n_neurons_motifs, n_synapses, n_neurons, n_input, n_output;
    int *motif_list;
    int simulation_type;

    int *synapse_matrix, *delay_list, *neuron_excitatory, *training_zones;
    float *weight_list;

    void *neuron_initializer; // function to initialize neurons
    void (*step)(); // functions to run a step on the simulation


    int i;

    // Load information from input parameters
    execution_type = strtoul(argv[1], NULL, 10);
    neuron_type = strtoul(argv[2], NULL, 10);
    simulation_type = strtoul(argv[3], NULL, 10);


    // load information about the snn from file
    load_network_information(argv[4], &n_neurons, &n_input, &n_output, &n_synapses, 
                            &synapse_matrix, &neuron_excitatory, &weight_list, &delay_list, &training_zones);



#ifdef DEBUG
    // print general information about the simulation / training
    printf("===================\n");
    printf("Execution properies:\n");
    printf(" - Execution type = %d\n", execution_type);
    printf(" - Neuron type for exectution = %d\n", neuron_type);
    printf(" - Simulation / training / testing = %d\n", simulation_type);
    printf(" - Number neurons = %d\n", n_neurons);
    printf(" - Number input neurons = %d\n", n_input);
    printf(" - Number output neurons = %d\n", n_output);
    printf(" - Number synapses = %d\n", n_synapses);
    printf(" - Neurons behaviour: ");
    print_array(neuron_excitatory, n_synapses);
    printf(" - Synapse matrix:\n");
    print_matrix(n_neurons, n_neurons, synapse_matrix);
    printf(" - Weights list: ");
    print_array_f(weight_list, n_synapses);
    printf(" - Delays list: ");
    print_array(delay_list, n_synapses);
    printf(" - Training zones: ");
    print_array(training_zones, n_synapses);
    printf("===================\n\n");

#endif


    // initialize snn structure
    spiking_nn_t snn;
    initialize_network(&snn, neuron_type, n_neurons, n_input, n_output, n_synapses, 
                        neuron_excitatory, synapse_matrix, weight_list, delay_list, training_zones);


#ifdef DEBUG
    // print information of network neurons
    printf("Printing neuron information...\n");
    for(i = 0; i<snn.n_neurons; i++){
        if(snn.neuron_type == 0){
            printf("Neuron %d: \n - V = %f\n - Excitatory = %d\n - Is input = %d\n - Is output = %d\n - n input synapses = %d\n - n output synapses = %d\n", 
                    i, snn.lif_neurons[i].v, snn.lif_neurons[i].excitatory, snn.lif_neurons[i].is_input_neuron, snn.lif_neurons[i].is_output_neuron, 
                    snn.lif_neurons[i].n_input_synapse, snn.lif_neurons[i].n_output_synapse);
        }
    }
    printf("===================\n\n");
#endif

#ifdef DEBUG
    // print information of network synapses
    printf("Printing neuron information...\n");
    for(i = 0; i<snn.n_synapses; i++)
        printf("Synapse %d: \n - W = %f\n - Delay = %d\n", 
                i, snn.synapses[i].w, snn.synapses[i].delay);
    printf("===================\n\n");
#endif


/*
    // Load snn network from file
    spiking_nn_t snn;


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
    //}

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
            printf("%f ", snn.lif_neurons[i].input_synapses[j]->w);
        }
        printf("Neuron %d input synapses checked!\n", i+1);
        printf("Checking output synapses...\n");
        for(int j = 0; j<snn.lif_neurons[i].output_synapse_amount; j++){
            //snn.lif_neurons[i].output_synapses[j].l_spike_times[10] = -1;
            printf("%f ", snn.lif_neurons[i].output_synapses[j]->w);
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
        for (int j = 0; j<=snn.lif_neurons[i].input_synapses[0]->last_spike; j++)
            printf("%d ", snn.lif_neurons[i].input_synapses[0]->l_spike_times[j]);
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
                float initial_w = snn.synapses[i].w;
                printf("Synapse %d, pre_t = %d, post_t = %d\n", i, snn.synapses[i].t_last_pre_spike, snn.synapses[i].t_last_post_spike);
                if(snn.synapses[i].t_last_post_spike != -1 && snn.synapses[i].t_last_pre_spike != -1)
                    add_stdp(&(snn.synapses[i]));
                printf("Synapse %d weight evolution: %f -> %f\n", i, initial_w, snn.synapses[i].w);
                snn.synapses[i].w = 0;
            }
        }
    }
    /*else{

    }*/

    /*// print spikes generated by each neuron
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
    }*/
    
    //free(snn.lif_neurons);

    return 0;
}