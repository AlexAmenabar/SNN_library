#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>

void print_matrix( int *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%d ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array(int *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
}

void print_matrix_f(float *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%f ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array_f(float *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%f ", array[i]);
    }
    printf("\n");
}

void random_input_spike_train_generator(const char *output_file_name, int time_steps, int prob){
    for(int i = 0; i<time_steps; i++){
        int p = rand() % 100;
        if(p > prob){
            // generate spike on i time step
        }
    }
}

void generate_random_synapse_matrix(int *synapse_matrix, int n_neurons, int n_input, int n_output, int *n_synapse, int *n_input_synapse, int *n_output_synapse){
    // bigger the number of synapses lower the probability: exponential distribution or pareto distribution
    int i, j;
    int max_synapses_between_neurons = 10;
    
    double u, lambda = 0.5;

    // initialize synapse amount data
    (*n_synapse) = 0; (*n_input_synapse) = 0; (*n_output_synapse) = 0;

    // generate synapse matrix
    for(i=0; i<n_neurons+1; i++){
        for(j=0; j<n_neurons+1; j++){
            if(i!=0 && j!=(n_neurons)){
                u = (double)rand() / max_synapses_between_neurons;
                synapse_matrix[i * (n_neurons + 1) + j] = (int)(-log(1-u) / lambda);
                
                // add synapse amount
                (*n_synapse) += synapse_matrix[i * (n_neurons + 1) + j];
                
                if(i==0)
                    (*n_input_synapse) = n_input_synapse[i * (n_neurons + 1) + j];
                
                if(j==n_neurons)
                    (*n_input_synapse) = n_output_synapse[i * (n_neurons + 1) + j];
            }
        }
    }
}


void generate_random_synaptic_weights(float *synaptic_weights, int n_neurons, int *synapse_matrix, int *neuron_behaviour_list){
    int i,j;

    int next_synapse = 0;

    // all network input synapses have positive weights
    for(i=0; i<n_neurons; i++){
        if(synapse_matrix[i]>0){
            synaptic_weights[next_synapse] = ((float)rand() / (float)(1.0)) * 3; 
            next_synapse ++;
        }
    }

    // rest of synapses
    for(i = 1; i<(n_neurons+1); i++){
        for(j = 0; j<(n_neurons+1); j++){ // last column synapses are network output synapses
            if(synapse_matrix[i * (n_neurons + 1)]>0){
                if(j == n_neurons){ // network output synapse: always positive
                    synaptic_weights[next_synapse] = ((float)rand() / (float)(1.0)) * 3; 
                }
                else{ // other synapses can be positive or negative depending on neuron
                    if(neuron_behaviour_list[i] == 1) // excitatory neuron
                        synaptic_weights[next_synapse] = ((float)rand() / (float)(1.0)) * 3; 
                    else // inhibitory neuron
                        synaptic_weights[next_synapse] = -((float)rand() / (float)(1.0)) * 3;
                }
                next_synapse ++;
            }
        }
    }
}

void generate_random_synaptic_delays(int *synaptic_delays, int n_neurons, int *synapse_matrix){
    int i,j;

    int next_synapse = 0;

    double u, lambda = 0.2;

    int max_delay = 20; // PROVISIONAL

    // input neurons delays are irrelevant
    for(i=0; i<n_neurons; i++){
        if(synapse_matrix[i]>0){
            synaptic_delays[next_synapse] = 1; 
            next_synapse ++;
        }
    }

    // rest of synapses
    for(i = 1; i<(n_neurons+1); i++){
        for(j = 0; j<(n_neurons+1); j++){ // last column synapses are network output synapses
            if(synapse_matrix[i * (n_neurons + 1)]>0){
                if(j == n_neurons) // network output synapse: delays are irrelevant
                    synaptic_delays[next_synapse] = 1; 
                else{// other synapses delays can be different
                    u = (double)rand() / max_delay;
                    synaptic_delays[next_synapse] = (int)log(1-u) / lambda;
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