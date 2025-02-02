#include "snn_library.h"
#include "network_generator.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int n, n_input, n_output, n_synapses, n_input_synapses, n_output_synapses;
    int *synapse_matrix, *synaptic_delays, *neuron_behaviour, *synaptic_learning_rules;
    float *synaptic_weights;

    n = strtoul(argv[1], NULL, 10);
    n_input = strtoul(argv[2], NULL, 10);
    n_output = strtoul(argv[3], NULL, 10);

    srand(time(NULL));

    printf("Input parameters: %d, %d, %d\n", n, n_input, n_output);


    printf("INPUT PARAMETERS READED\n");

    synapse_matrix = (int *)malloc((n+1) * (n+1) * sizeof(int));
    neuron_behaviour = (int *)malloc(n * sizeof(int));
    generate_random_synapse_matrix(synapse_matrix, n, n_input, n_output, &n_synapses, &n_input_synapses, &n_output_synapses);
    printf("SYNAPSE MATRIX GENERATED\n");
    
    generate_random_neuron_behaviour(neuron_behaviour, n);
    printf("NEURON BEHAVIOUR LIST GENERATED\n");

    synaptic_delays = (int *)malloc(n_synapses * sizeof(int));
    synaptic_weights = (float *)malloc(n_synapses * sizeof(float));
    synaptic_learning_rules = (int *)malloc(n_synapses * sizeof(int));
    printf("MEMORY RESERVED FOR SYNAPTIC PPROPERTIES\n");
   
   
    generate_random_synaptic_weights(synaptic_weights, n, synapse_matrix, neuron_behaviour);
    printf("SYNAPTIC WEIGHTS GENERATED\n");

    generate_random_synaptic_delays(synaptic_delays, n, synapse_matrix);
    printf("SYNAPTIC DELAYS GENERATED\n");

    generate_random_training_type(synaptic_learning_rules, n_synapses);
    printf("SYNAPTIC LEARNING RULES GENERATED\n");


    // store generated network information on a file
    FILE *f;
    f = fopen("new_snn.txt", "w");
 
    fprintf(f, "%d", n);
    fprintf(f, "\n");
    fprintf(f, "%d", n_input);
    fprintf(f, "\n");
    fprintf(f, "%d", n_output);
    fprintf(f, "\n");

    fprintf(f, "%d", n_synapses);
    fprintf(f, "\n");
    fprintf(f, "%d", n_input_synapses);
    fprintf(f, "\n");
    fprintf(f, "%d", n_output_synapses);
    fprintf(f, "\n");

    // store neurons behaviour
    for(int i = 0; i<n; i++)
        fprintf(f, "%d ", neuron_behaviour[i]);
    fprintf(f, "\n\n");

    // write synaptic connections
    for(int i=0; i<(n+1); i++){
        for(int j=0; j<(n+1); j++)
            fprintf(f, "%d ", synapse_matrix[i*(n+1)+j]);
        fprintf(f, "\n");
    }
    fprintf(f, "\n");

    // write weights
    for(int i = 0; i<n_synapses; i++)
        fprintf(f, "%f ", synaptic_weights[i]);
    fprintf(f, "\n\n");


    // write weights
    for(int i = 0; i<n_synapses; i++)
        fprintf(f, "%d ", synaptic_delays[i]);
    fprintf(f, "\n\n");

    // write weights
    for(int i = 0; i<n_synapses; i++)
        fprintf(f, "%d ", synaptic_learning_rules[i]);
    fprintf(f, "\n\n");


    fclose(f);
}