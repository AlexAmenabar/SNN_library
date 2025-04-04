#ifndef SNN_LIBRARY_H
#define SNN_LIBRARY_H


// max spikes to store per synapse
#ifndef MAX_SPIKES
#define MAX_SPIKES 20
#endif

#ifndef INPUT_MAX_SPIKES
#define INPUT_MAX_SPIKES 4000
#endif

/**
 * General structures
 */

 /// Dynamic list to store generated spikes
typedef struct spike{
    int t;
    struct spike *next; 
} spike_t;


/// @brief LIF neuron model structure to store information about the neuron
typedef struct {
    //synapse_t **input_synapses; // lists of input and output synapses
    int *input_synapse_indexes; // indexes to get neuron input synapses from snn.synapses
    int n_input_synapse;
    int next_input_synapse;

    //synapse_t **output_synapses;
    int *output_synapse_indexes; // indexes to get neuron output synapses from snn.synapses
    int n_output_synapse;
    int next_output_synapse;

    // is input ot output neuron
    int is_input_neuron;
    int is_output_neuron;

    int excitatory; // 0 if inhibitory, else 1

    float v; // if at start it's postive, excitatory neuron, else inhibitory
    float r;// = 1.0; // membrane resistance
    float v_rest;// = 0;
    float v_reset;// = 0;
    float v_tresh;// = 1;

    int r_t; //refractory time

    int t_last_spike; // time step when last spike has been generated by this neuron
} lif_neuron_t;

/// @brief Structure to store all the information about the synapses
typedef struct {
    float w; // if at start it is positive, excitatory, else inhibitory
    int delay; // the delay time of the synapse

    int *l_spike_times; // FIFO list
    int next_spike, last_spike, max_spikes; // indexes to control list

    void (*learning_rule)(); // pointer to learning rule function (STDP type)

    int t_last_pre_spike, t_last_post_spike; // time steps of the last spikes generated by synapse pre synaptic neuron and post synaptic neuron
    int pre_neuron_computed, post_neuron_computed; // used to know when to compute STDP

    lif_neuron_t *pre_synaptic_lif_neuron;
    lif_neuron_t *post_synaptic_lif_neuron;
} synapse_t;


/// @brief Structure to store motif types
typedef struct {
    int type; 
    void *neurons; // each motif can have a different amount of neurons depending on the type 
} motif_t;


/// @brief SNN structure to store all the information about the network
typedef struct {
    // network general information
    int n_neurons; // n neurons on the network
    int n_input; // number of input neurons
    int n_output; // number of output neurons

    int neuron_type; // type of the neurons of this network

    int n_synapses; // number of synapses of the network
    int n_input_synapses; // number of network input synapses
    int n_output_synapses; // number of network output synapses

    // pointers to different neuron types (one used on execution)
    lif_neuron_t *lif_neurons; 
    //hh_neuron_t *hh_neurons;

    // pointer to network synapses
    synapse_t *synapses;

    // hyperparameters
} spiking_nn_t;


typedef struct{
    int *spike_times; // list with times in which a spike has been generated
    int n_spikes; // number of spikes
} spike_train_t;

/**
 * General function definition
 */


/// @brief Initialize network neurons 
/// @param snn SNN structure with neuron list
/// @param neuron_behaviour_list List of neuron types (excitatory or inhibitory)
/// @param synapse_matrix Synapse matrix
void initialize_network_neurons(spiking_nn_t *snn, int *neuron_behaviour_list, int **synaptic_connections);


/// @brief Initialize a synapse data
/// @param synapse Synapse to be initialized
/// @param w Weight to set to the synapse
/// @param delay Delay to set to the synapse
/// @param training Training type to set to the synapse
void initialize_synapse(synapse_t *synapse, float w, int delay, int training, spiking_nn_t *snn, int synapse_id);

void re_initialize_synapse(synapse_t *synapse);



/// @brief Initialize network synapses
/// @param snn SNN structure with synapse list
/// @param n_synapses Number of synapses
/// @param weight_list List of synaptic weights
/// @param delay_list List of synaptic delays
/// @param training_zones List of training type for each synapse
void initialize_network_synapses(spiking_nn_t *snn, int n_synapses, float *weight_list, int *delay_list, int *training_zones);


/// @brief Add input synapse to a neuron
/// @param snn SNN structure
/// @param neuron_index Index of the neuron to add a synapse
/// @param synapse_index Index of the synapse to be added as input to neuron
void add_input_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index);


/// @brief Add output synapse to a neuron
/// @param snn SNN structure
/// @param neuron_index Index of the neuron to add a synapse
/// @param synapse_index Index of the synapse to be added as output to neuron
void add_output_synapse_to_neuron(spiking_nn_t *snn, int neuron_index, int synapse_index);


/// @brief Connect neuron to input and output synapses
/// @param snn SNN structure
/// @param synapse_matrix Synapse matrix 
void connect_neurons_and_synapses(spiking_nn_t *snn, int **synaptic_connections);


/// @brief Initialize spiking neural network structure
/// @param snn Structure to initialize
/// @param neuron_type Type of neuron of this simulation
/// @param n_neurons Number of neurons
/// @param n_input Number of neurons on the input layer
/// @param n_output Number of neurons on the output layer
/// @param n_synapses Number of synapses
/// @param n_input_synapses Number of network input synapses
/// @param n_output_synapses Nummber of network output synapses
/// @param neuron_behaviour_list List with neuron types (excitatory or inhibitory)
/// @param synapse_matrix Matrix of synaptic connections
/// @param weight_list List of synaptic weights
/// @param delay_list List of synaptic delays
/// @param training_zones List of training types for each synapse
void initialize_network(spiking_nn_t *snn, int neuron_type, int n_neurons, int n_input, int n_output, int n_synapses, int n_input_synapses, int n_output_synapses, 
                    int *neuron_behaviour_list, int **synaptic_connections, float *weight_list, int *delay_list, int *training_zones);

#endif
