#include "network_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void generate_random_synapse_matrix(int **synaptic_connections, int n_neurons, int n_input, int n_output, int *n_synapse, int *n_input_synapse, int *n_output_synapse){
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;
    int max_synapses_between_neurons = 10;
    
    double u, lambda = 0.8;

    // initialize synapse amount data
    *n_synapse = 0; *n_input_synapse = 0; *n_output_synapse = 0;

    // generate synapse matrix
    for(i=0; i<(n_neurons+1); i++){
        for(j=0; j<n_)
    }

    for(i=0; i<(n_neurons+1); i++){
        for(j=0; j<(n_neurons+1); j++){
            //printf("i = %d, j = %d, %d\n", i, j, i*(n_neurons+1)+j);
            if(i==0 && j!=(n_neurons) || i!=0 && j==(n_neurons) || i>0 && j<=n_neurons){
                u = (double)rand() / RAND_MAX;
                synapse_matrix[i * (n_neurons + 1) + j] = (int)(-log(1-u) / lambda);
                
                // add synapse amount
                *n_synapse += synapse_matrix[i * (n_neurons + 1) + j];
                
                if(i==0)
                    *n_input_synapse += synapse_matrix[i * (n_neurons + 1) + j];
                
                if(j==n_neurons)
                    *n_output_synapse += synapse_matrix[i * (n_neurons + 1) + j];
            }
            else{
                synapse_matrix[i * (n_neurons+1) + j] = 0;
            }
        }
    }
}


void generate_random_synaptic_weights(float *synaptic_weights, int n_neurons, **synaptic_connections, int *neuron_behaviour_list){
    int i,j, l;

    int next_synapse = 0;

    // all network input synapses have positive weights
    for(i=0; i<n_neurons; i++){
        for(j = 0; j<synapse_matrix[i]; j++){
            int valid = 0;
            float w = 0;
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
    for(i = 1; i<(n_neurons+1); i++){
        for(j = 0; j<(n_neurons+1); j++){ // last column synapses are network output synapses
            for(l=0; l<synapse_matrix[i * (n_neurons + 1) + j]; l++){
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

void generate_random_synaptic_delays(int *synaptic_delays, int n_neurons, int **synaptic_connections){
    int i,j, l;

    int next_synapse = 0;

    double u, lambda = 0.6;

    int max_delay = 20; // PROVISIONAL

    // input neurons delays are irrelevant
    for(i=0; i<n_neurons; i++){
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
    }
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