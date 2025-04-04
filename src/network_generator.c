#include "network_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "load_data.h"


void generate_semi_random_synaptic_connections(int **synaptic_connections, int n_neurons, int n_input, int n_output, int *n_synapse, int *n_input_synapse, int *n_output_synapse){ // por ahora n_input_synapse y n_output_synapse no se utilizan, ya que por neurona de entrada/salida hay una unica sinapsis de entrada/salida
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;

    // amount of connection per neuron are computed
    int connections_per_neuron = ((*n_synapse) - (*n_input_synapse) - (*n_output_synapse)) / n_neurons; // neuronas que hay entre medias en la red
    int generated_connections = 0; 

    // alocate memory
    synaptic_connections[0] = (int *)malloc((n_input * 2 +1 ) * sizeof(int));
    for(i=0; i<n_neurons+1; i++){
        if(i>n_neurons-n_output)
            synaptic_connections[i] = (int *)malloc(((connections_per_neuron * 2 + 2) + 1) * sizeof(int));
        else
            synaptic_connections[i] = (int *)malloc(((connections_per_neuron * 2) + 1) * sizeof(int));
    }


    // generate network input synaptic connections
    int next_pos = 1; 
    synaptic_connections[0][0] = n_input;
    for(i = 0; i<n_input; i++){
        int neuron_id = i;
        synaptic_connections[0][next_pos] = neuron_id;
        synaptic_connections[0][next_pos + 1] = 1;
        next_pos += 2;
    }

    // generate network synapses
    for(i = 1; i<n_neurons+1; i++){ 
        next_pos = 1; 
        for(j = 0; j<connections_per_neuron; j++){
            int neuron_id = rand() % n_neurons; // neuron to be connected to
            //if(neuron_id > n_neurons - connections_per_neuron + j)
            //    neuron_id = n_neurons - connections_per_neuron + j;
            synaptic_connections[i][next_pos] = neuron_id;
            synaptic_connections[i][next_pos + 1] = 1;
            next_pos += 2;
        }
        synaptic_connections[i][0] = connections_per_neuron;
        // add output connection if neuron is output
        if(i > n_neurons - n_output){
            synaptic_connections[i][next_pos] = -1;
            synaptic_connections[i][next_pos + 1] = 1;
            next_pos += 2;
            
            synaptic_connections[i][0] += 1;
        }
    }
}

void generate_random_synaptic_connections(int **synaptic_connections, int n_neurons, int n_input, int n_output, int *n_synapse, int *n_input_synapse, int *n_output_synapse, double lambda){
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;
    int max_synapses_between_neurons = 10;
    
    double u;//1.5;

    printf("lambda: %f\n", lambda);
    // initialize synapse amount data
    *n_synapse = 0; *n_input_synapse = 0; *n_output_synapse = 0;

    // pointer to store synaptic connections
    int *temp_synaptic_connections = (int *)malloc((1 + 2 * n_neurons + 2) * sizeof(int)); // 1 is the number of synaptic connections, for each neuron 2 numbers (worst case) and another two if neuron is network output
    int number_neuron_connections=0; // variable to store to how much neurons is conected a neurons (not synaptic connections)
    int synapses; // generated synapse amount from neuron i to j
    int next_pos = 1; 

    // generate network input synaptic connections
    for(i = 0; i<n_input; i++){ 
        u = (double)rand() / RAND_MAX;
        synapses = (int)(-log(1-u) / lambda) / 2;

        // add
        if(synapses == 0){
            synapses = 1; // input neurons must have at least one input synapse
        }
        synapses = 1;
        temp_synaptic_connections[next_pos] = i; // network input synapse to neuron i
        temp_synaptic_connections[next_pos+1] = synapses; // amount of synapses 
        next_pos += 2;
        number_neuron_connections +=1; // total input synapses
        
        // add synapse amount to total synapse number and to input synapse number
        *n_input_synapse += synapses;
        *n_synapse += synapses;
    }
    temp_synaptic_connections[0] = number_neuron_connections;

    // allocate memory for complete synaptic connections variable
    synaptic_connections[0] = (int *)malloc((temp_synaptic_connections[0] * 2 + 1) * sizeof(int));

    // write on file (less memory) and copy to general
    for(i = 0; i<(temp_synaptic_connections[0] * 2 + 1); i++)
        synaptic_connections[0][i] = temp_synaptic_connections[i];


    // generate synaptic connections
    for(i=1; i<(n_neurons+1); i++){ // network input synapses not taken into account
        next_pos = 1;
        number_neuron_connections = 0;

        // for each neuron, generate synapses
        for(j=0; j<(n_neurons); j++){ // j == n_neurons --> network output synapses
            u = (double)rand() / RAND_MAX;
            synapses = (int)(-log(1-u) / lambda) / 2;

            if(synapses>0){
                /*if(j==n_neurons){ // network output synapses
                    temp_synaptic_connections[next_pos] = -1; // network input synapse to neuron i
                    temp_synaptic_connections[next_pos+1] = synapses; // amount of synapses 
                    next_pos += 2;
                    number_neuron_connections +=1; // neuron is connected to one more neuron
                    *n_output_synapse += synapses; 
                }
                else{*/
                temp_synaptic_connections[next_pos] = j; // network input synapse to neuron j
                temp_synaptic_connections[next_pos+1] = synapses; // amount of synapses 
                next_pos += 2;
                number_neuron_connections +=1; // neuron is connected to one more neuron
                //}
                *n_synapse += synapses;
            }            
        }

        // if neuron is output neuron
        if(i > n_neurons - n_output){
            temp_synaptic_connections[next_pos] = -1; // network input synapse to neuron i
            temp_synaptic_connections[next_pos+1] = 1; // for now only one synapse as output
            next_pos += 2;
            number_neuron_connections += 1; // neuron is connected to one more neuron
            
            // add synpase amount to output synapses and total synapses
            *n_output_synapse += 1; 
            *n_synapse += 1;
        }
        temp_synaptic_connections[0] = number_neuron_connections;

        // allocate memory for complete synaptic connections variable
        synaptic_connections[i] = (int *)malloc((temp_synaptic_connections[0] * 2 + 1) * sizeof(int));

        // write on file (less memory) and copy to general
        for(j = 0; j<(temp_synaptic_connections[0] * 2 + 1); j++)
            synaptic_connections[i][j] = temp_synaptic_connections[j];
    }
}


void generate_random_synaptic_weights(float *synaptic_weights, int n_neurons, int **synaptic_connections, int *neuron_behaviour_list){
    int i,j, l;

    int next_synapse = 0, valid;
    float w;

    // all network input synapses have positive weights
    for(i=1; i<(synaptic_connections[0][0] * 2 + 1); i+=2){
        for(j = 0; j<(synaptic_connections[0][i+1]); j++){
            valid = 0;
            w = 0;
            while(valid == 0){
                w = ((float)rand() / (float)(RAND_MAX)) * 5;
                if(w>0.005) valid = 1;
            }
            //printf("generated w %f\n", w);

            synaptic_weights[next_synapse] = (float)w; 
            next_synapse ++;
        }
    }

    // rest of synapses
    for(i=1; i<(n_neurons + 1); i++){
        for(j=1; j<(synaptic_connections[i][0] * 2 +1); j+=2){
            for(l = 0; l<synaptic_connections[i][j+1]; l++){
                // generate random value
                int valid = 0;
                float w = 0;
                while(valid == 0){
                    w = ((float)rand() / (float)(RAND_MAX)) * 5;
                    if(w>0.005) valid = 1;
                }
                //printf("generated w %f\n", w);
                if(j == n_neurons){ // network output synapse: always positive
                    synaptic_weights[next_synapse] = (float)w; 
                }
                else{ // other synapses can be positive or negative depending on neuron
                    if(neuron_behaviour_list[i-1] == 1){ // excitatory neuron
                        synaptic_weights[next_synapse] = (float)w; 
                    }
                    else{ // inhibitory neuron
                        synaptic_weights[next_synapse] = (float)-w;
                    }
                }
                next_synapse ++;
            }
        }
    }
}

void generate_random_synaptic_delays(int *synaptic_delays, int n_synapses, int n_input_synapses, int n_output_synapses){//int n_neurons, int **synaptic_connections){
    int i,j, l;

    int next_synapse = 0;

    double u, lambda = 0.6;

    int max_delay = 20; // PROVISIONAL

    for(next_synapse = 0; next_synapse<n_input_synapses; next_synapse++)
        synaptic_delays[next_synapse] = 1;

    
    for(next_synapse = n_input_synapses; next_synapse<n_synapses; next_synapse++){
        synaptic_delays[next_synapse] = 1;
        u = (double)rand() / RAND_MAX;
        synaptic_delays[next_synapse] = (int)(-log(1-u) / lambda) % max_delay;
        if(synaptic_delays[next_synapse] == 0) synaptic_delays[next_synapse]++; // delay can not be 0
    }

    // input neurons delays are irrelevant
    /*for(i=0; i<n_neurons; i++){
        for(j = 0; j<synapse_matrix[i]; j++){
            synaptic_delays[next_synapse] = 1; 
            next_synapse ++;
        }
    }

    // rest of synapses
    for(i = 1; i<(n_neurons+1); i++){
        for(j = 0; j<(n_neurons+1); j++){ // last column synapses are network output synapses
            for(l=0; l<synapse_matrix[i * (n_neurons + 1) + j]; l++){
                if(j == n_neurons) // network output synapse: delays are irrelevant
                    synaptic_delays[next_synapse] = 1; 
                else{// other synapses delays can be different
                    u = (double)rand() / RAND_MAX;
                    synaptic_delays[next_synapse] = (int)(-log(1-u) / lambda) % max_delay;
                    if(synaptic_delays[next_synapse] == 0) synaptic_delays[next_synapse] = 1;
                }
                next_synapse ++;
            }
        }
    }*/
}

void generate_random_training_type(int *synaptic_learning_rule, int n_synapses){
    int i,j;

    int next_synapse = 0;
    int learning_rules = 3;

    for(i = 0; i<n_synapses; i++){
        synaptic_learning_rule[i] = rand() % learning_rules;
    }
}

void generate_random_neuron_behaviour(int *neurons_behaviour, int n){
    int i;

    for(i = 0; i<n; i++){
        int rand_number = rand() % 100;
        if(rand_number > 75){
            neurons_behaviour[i] = 0;
        }
        else{
            neurons_behaviour[i] = 1;
        }
    }
}



void generate_random_snn(){
    
}