#include "nsga2.h"



// Functions to initialize each type of motif (CHAPUCILLA)
void initialize_FFE(motif_t *motif){
    motif->neuron_behaviour[0] = 1; // excitatory
    motif->neuron_behaviour[1] = 1; // excitatory

    motif->n_synapses = 1;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->output_neurons[0] = 1;
    
    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[1] = 1;
    motif->n_output_synapses_per_neuron[0] = 1;
}

void initialize_FBE(motif_t *motif){
    motif->neuron_behaviour[0] = 1; // excitatory
    motif->neuron_behaviour[1] = 1; // excitatory

    motif->n_synapses = 1;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;
    motif->connectivity_matrix[2] = 1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->output_neurons[0] = 1;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[0] = 1;
    motif->n_input_synapses_per_neuron[1] = 1;

    motif->n_output_synapses_per_neuron[0] = 1;
    motif->n_output_synapses_per_neuron[1] = 1;
}

void initialize_FBI(motif_t *motif){
    motif->neuron_behaviour[0] = 1; // excitatory
    motif->neuron_behaviour[1] = 1; // excitatory
    motif->neuron_behaviour[2] = -1; // inhibitory

    motif->n_synapses = 3;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;
    motif->connectivity_matrix[5] = 1;
    motif->connectivity_matrix[6] = -1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->output_neurons[0] = 2;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[0] = 1;
    motif->n_input_synapses_per_neuron[1] = 1;
    motif->n_input_synapses_per_neuron[2] = 1;

    motif->n_output_synapses_per_neuron[0] = 1;
    motif->n_output_synapses_per_neuron[1] = 1;
    motif->n_output_synapses_per_neuron[2] = 1;
}

void initialize_RCE(motif_t *motif){
    motif->neuron_behaviour[0] = 1; // excitatory
    motif->neuron_behaviour[1] = 1; // excitatory
    motif->neuron_behaviour[2] = -1; // inhibitory

    motif->n_synapses = 4;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;
    motif->connectivity_matrix[6] = 1;
    motif->connectivity_matrix[11] = 1;
    motif->connectivity_matrix[12] = 1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->input_neurons[1] = 1;
    motif->input_neurons[2] = 2;
    motif->input_neurons[3] = 3;

    motif->output_neurons[0] = 0;
    motif->output_neurons[1] = 1;
    motif->output_neurons[2] = 2;
    motif->output_neurons[3] = 3;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[0] = 1;
    motif->n_input_synapses_per_neuron[1] = 1;
    motif->n_input_synapses_per_neuron[2] = 1;
    motif->n_input_synapses_per_neuron[3] = 1;

    motif->n_output_synapses_per_neuron[0] = 1;
    motif->n_output_synapses_per_neuron[1] = 1;
    motif->n_output_synapses_per_neuron[2] = 1;
    motif->n_output_synapses_per_neuron[3] = 1;
}

void initialize_RCI(motif_t *motif){
    motif->n_synapses = 4;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = -1;
    motif->connectivity_matrix[6] = -1;
    motif->connectivity_matrix[11] = -1;
    motif->connectivity_matrix[12] = -1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->input_neurons[1] = 1;
    motif->input_neurons[2] = 2;
    motif->input_neurons[3] = 3;

    motif->output_neurons[0] = 0;
    motif->output_neurons[1] = 1;
    motif->output_neurons[2] = 2;
    motif->output_neurons[3] = 3;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[0] = 1;
    motif->n_input_synapses_per_neuron[1] = 1;
    motif->n_input_synapses_per_neuron[2] = 1;
    motif->n_input_synapses_per_neuron[3] = 1;

    motif->n_output_synapses_per_neuron[0] = 1;
    motif->n_output_synapses_per_neuron[1] = 1;
    motif->n_output_synapses_per_neuron[2] = 1;
    motif->n_output_synapses_per_neuron[3] = 1;
}

void initialize_LTI(motif_t *motif){
    motif->n_synapses = 2;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;
    motif->connectivity_matrix[5] = -1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->input_neurons[1] = 2;

    motif->output_neurons[0] = 0;
    motif->output_neurons[1] = 2;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[1] = 1;
    motif->n_input_synapses_per_neuron[2] = 1;

    motif->n_output_synapses_per_neuron[0] = 1;
    motif->n_output_synapses_per_neuron[1] = 1;
}

void initialize_CPG(motif_t *motif){
    motif->n_synapses = 5;

    // initialize conectivity matrix
    motif->connectivity_matrix[1] = 1;
    motif->connectivity_matrix[2] = -1;
    motif->connectivity_matrix[3] = 1;
    motif->connectivity_matrix[5] = 1;
    motif->connectivity_matrix[6] = 1;
    motif->connectivity_matrix[7] = 1;

    // initialize in and out neurons
    motif->input_neurons[0] = 0;
    motif->input_neurons[1] = 1;
    motif->input_neurons[2] = 2;

    motif->output_neurons[0] = 0;
    motif->output_neurons[1] = 1;
    motif->output_neurons[2] = 2;

    // initialize number of input and output synapses per neuron
    motif->n_input_synapses_per_neuron[0] = 2;
    motif->n_input_synapses_per_neuron[1] = 2;
    motif->n_input_synapses_per_neuron[2] = 2;

    motif->n_output_synapses_per_neuron[0] = 2;
    motif->n_output_synapses_per_neuron[1] = 2;
    motif->n_output_synapses_per_neuron[2] = 2;
}

void initialize_motif(motif_t *motif){
    // redirect

    // initialize connectivity matrix with 0s
    for(int i = 0; i<motif->n_neurons * motif->n_neurons; i++){
        motif->connectivity_matrix[i] = 0;
    }

    switch(motif->motif_type){
        case 0:
            initialize_FFE(motif);
            break;
        case 1:
            initialize_FBE(motif);
            break;
        case 2:
            initialize_FBI(motif);
            break;
        case 3:
            initialize_RCE(motif);
            break;
        case 4:
            initialize_RCI(motif);
            break;
        case 5:
            initialize_LTI(motif);
            break;
        case 6:
            initialize_CPG(motif);
            break;
    }
}

// General function to initialize motifs
void initialize_motifs(NSGA2Type *nsga2Params, void *, void *){
    // allocate memory for motifs list
    int i, j, l;
    
    n_motifs = 7; // existing motifs
    motifs = (motif_t *)malloc(n_motifs * sizeof(motif_t));

    // allocate memory for each motif
    for(i = 0; i<n_motifs; i++){
        motifs[i].motif_type = i; // initialize type of motif
    }

    // initialize some parameters
    motifs[0].n_neurons = 2;
    motifs[0].n_input = 1;
    motifs[0].n_output = 1;

    motifs[1].n_neurons = 2;
    motifs[1].n_input = 1;
    motifs[1].n_output = 1;

    motifs[2].n_neurons = 3;
    motifs[2].n_input = 1;
    motifs[2].n_output = 1;

    motifs[3].n_neurons = 4;
    motifs[3].n_input = 4;
    motifs[3].n_output = 4;

    motifs[4].n_neurons = 4;
    motifs[4].n_input = 4;
    motifs[4].n_output = 4;

    motifs[5].n_neurons = 3;
    motifs[5].n_input = 2;
    motifs[5].n_output = 2;

    motifs[6].n_neurons = 3;
    motifs[6].n_input = 3;
    motifs[6].n_output = 3;
    
    // allocate memory for neuron behaviours and connectivity  
    for(i=0; i<n_motifs; i++){
        motifs[i].neuron_behaviour = (int *)malloc(motifs[i].n_neurons * sizeof(int));
        motifs[i].connectivity_matrix = (int *)malloc(motifs[i].n_neurons * motifs[i].n_neurons * sizeof(int));
        
        // allocate memory for input and output synapses numbers for each neuron
        motifs[i].n_input_synapses_per_neuron = (int *)calloc(motifs[i].n_neurons, sizeof(int)); // ititialize with 0 values
        motifs[i].n_output_synapses_per_neuron = (int *)calloc(motifs[i].n_neurons, sizeof(int)); // initialize with 0 values

        // allocate memory for input and output neurons identifiers
        motifs[i].input_neurons = (int *)malloc(motifs[i].n_input * sizeof(int));
        motifs[i].output_neurons = (int *)malloc(motifs[i].n_output * sizeof(int));
    }

    // initialize motifs
    for(i = 0; i<n_motifs; i++){
        initialize_motif(&(motifs[i]));
    }
}
