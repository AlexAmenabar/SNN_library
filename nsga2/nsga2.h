/* This file contains the variable and function declarations */


# ifndef NSGA_INCLUDED    
# define NSGA_INCLUDED    

#ifndef NSGA2_H
# define NSGA2_H

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <unistd.h>
# include <stdint.h> // uint8_t
# include <string.h>

# include "snn_library.h"
# include "encoders/image_encoders.h"


# define INF 1.0e14
# define EPS 1.0e-14
# define eul  2.71828182845905
# define pi 3.14159265358979
# define GNUPLOT_COMMAND "gnuplot -persist"


typedef struct
{
    int *array; // array of numbers
    int n;
}
int_array_t;


// array of int_arrays
typedef struct
{
    int_array_t *int_array; // array of int_arrays
    int n;
}
int_array_list_t;


// In this struct the internal structure of each motif type is stored <-- ONLY INTERNAL STRUCTURE INFORMATION
// this struct is used to avoid to repeat information
typedef struct
{
    int motif_type;
    int n_neurons;
    int n_synapses;
    int *neuron_behaviour;
    int *connectivity_matrix;

    int n_input, n_output; // number of input and output neurons in the motif
    int *input_neurons; // input neurons
    int *output_neurons; // output neurons

    // number of input and output synapses per each neuron
    int *n_input_synapses_per_neuron;
    int *n_output_synapses_per_neuron;
}
motif_t; // this struct is used to store existing motifs, not those
        // of the networks used in the genetic algorithm


// The list of synapses is orderes following the input criterion for neurons
typedef struct sparse_matrix_node_t
{
    uint32_t row, col;
    int8_t value;

    uint8_t *latency; //latency of the synaptic connection (is a list if value is bigger than 1)
    double *weight;
    uint8_t *learning_rule;

    // next element of the sparse matrix
    struct sparse_matrix_node_t *next_element;
}
sparse_matrix_node_t;

// Struct that contains helper information to build the sparse matrix
typedef struct sparse_matrix_build_info_t
{
    int actual_motif_index, actual_motif_type, actual_motif_first_neuron_index, actual_neuron_local_index;
    motif_t *actual_motif_info;
    int actual_motif_proccessed;
    
    int input_motif_index, input_motif_type, input_motif_first_neuron_index;
    motif_t *input_motif_info;

    int previous_input_motif_index, next_input_motif_index, n_connections;

    int *motifs_types, *motifs_first_neuron_indexes;

    // for removing
    sparse_matrix_node_t *previous_synapse_node;
}
sparse_matrix_build_info_t;


typedef struct neuron_node_t
{
    double threshold;
    uint8_t refract_time;
    double v_rest;
    float r;
    int behaviour;

    // next neuron
    struct neuron_node_t *next_neuron;
}
neuron_node_t;

// MEW
typedef struct new_motif_t
{
    uint motif_id; 
    uint8_t motif_type;
    uint32_t initial_global_index; // index of the first neuron in the neuron list
    
    neuron_node_t *first_neuron; // n neurons can be gotten from motifs general structures
    struct new_motif_t *next_motif; // pointer to the next motif of the list
    
    // The variables above could be used in the future to optimize the implementation
    //struct sparse_matrix_node_t *first_input_synapse; // pointer to the first input synapse of the first neuron
    //int_array_t *input_motifs;
    //int_array_t *output_motifs; 
}
new_motif_t;

// NEW
typedef struct learning_zone_t
{
    int stdp_type;
    int n_motifs;
    new_motif_t *first_motif;

    struct learning_zone_t *next_zone;
}
learning_zone_t;

typedef struct int_node_t
{
    int value;
    struct int_node_t *next;
    struct int_node_t *prev;
}
int_node_t;

typedef struct int_dynamic_list_t{
    int n_nodes;
    struct int_node_t *first_node;
}
int_dynamic_list_t;


typedef struct ind_connectivity_info_t
{
    struct int_dynamic_list_t *in_connections; // a dynamic list for each motif
    struct int_dynamic_list_t *out_connections; // a dynamic list for each motif
    int n_max_motifs; // the size of the in_connections and out_connections arrays
}
ind_connectivity_info_t;


typedef struct
{
    int rank;
    double constr_violation;
    double *xreal;
    int **gene;
    double *xbin;
    double *obj;
    double *constr;
    double crowd_dist;

    // structure to store the spiking neural network
    spiking_nn_t *snn; // decoded
    
    int n_motifs; // number of motifs for the individual

    // network number of motifs, neurons and synapses
    int n_neurons; // total amount of neurons for the network
    int n_input_neurons;
    
    int n_synapses; // total amount of synapses for the network
    int n_input_synapses; // network number of input synapses

    // dynamic list of neurons
    neuron_node_t *neurons;

    // sparse matrix of connections (dynamic list)
    sparse_matrix_node_t *input_synapses; // list that indicates which ones are the network input synapses
    sparse_matrix_node_t *connectivity_matrix; // connectivity sparse matrix: first pointer for rows, second one for columns, and the third one for values. It is a ordered list
    // int8_t as there can be negative and positive values (sign indicates if the weight must be positive or negative) and the value indicates the amount of connections between two neurons 


    // NEW
    int n_learning_zones;
    learning_zone_t *learning_zones;
    new_motif_t *motifs_new;

    struct ind_connectivity_info_t connectivity_info;
}
individual;

typedef struct
{
    individual *ind;
}
population;

// I think this is a dynamic list used for something I don't understand in this moment
typedef struct lists
{
    int index;
    struct lists *parent;
    struct lists *child;
} list;

typedef struct selected_samples_info_t
{
    int *sample_indexes; // selected sample indexes in the global dataset
    int *labels; // selected samples labels
    int *n_selected_samples_per_class; // number of selected samples for each class
    int **sample_indexes_per_class; // indexes of selected samples for each class, in the sample_indexes
} selected_samples_info_t;


typedef struct NSGA2Type
{
    // general variables
    double seed; // seed to randomize execution
    int n_process;
    int popsize; // population size
    int nobj; // number of objective functions
    
    // added by me
    int neuron_type;
    int max_motifs, min_motifs;
    int max_multiple_connections, min_multiple_connections; // connections between motifs
    double pcross;
    double pmut;
    // finish added by me


    // probably I won't use these variables
    int nreal; // number of real (numbers) variables
    int nbin; // number of binary variables
    int ncon; // number of optimization problem constraints
    double pcross_real; // probability of crossover of real variables
    double pcross_bin; // probability of crossover of binary variables
    double pmut_real; // probability of mutation for real variables
    double pmut_bin; // probability of mutation for binary variables
    double eta_c;
    double eta_m;
    int ngen; // number of generations
    int nbinmut;
    int nrealmut;
    int nbincross;
    int nrealcross;
    int *nbits; // number of bits for binary variable
    double *min_realvar; // minimum real number
    double *max_realvar; // maximum real number
    double *min_binvar; // minumum binary variable
    double *max_binvar; // maximum binary variable
    int bitlength;
    int choice;
    int obj1;
    int obj2;
    int obj3;
    int angle1;
    int angle2;

    // added by me
    // variables for neurons
    double max_vthres, min_vthres;
    double max_vrest, min_vrest;
    int max_R, min_R;
    int max_refracttime, min_refracttime;

    // variables for synapes
    int max_latency, min_latency;
    int max_learning_rule, min_learning_rule;
    double max_weight, min_weight;

    // dataset variables
    int dataset_type; // for the future, now only images
    char train_dataset_dir[500];
    char train_labels_dir[500];
    char test_dataset_dir[500];
    char test_labels_dir[500];
    int n_train_samples;
    int n_test_samples;
    int n_classes;
    int image_size;
    int bins;

    int mode;
    int n_samples;
    int n_repetitions;

    double max_neurons, min_neurons;
    double max_synapses, min_synapses;
    double max_motifs_add, min_motifs_add;
    double max_motifs_remove, min_motifs_remove;

    char obj_values_dir[500];
    char classification_dir[500];
    char **individuals_dir; // [n_individuals x 500]
} NSGA2Type;

// Global 
extern FILE *fpt1;
extern FILE *fpt2;
extern FILE *fpt3;
extern FILE *fpt4;
extern FILE *fpt5;
extern FILE *gp;
extern population *parent_pop;
extern population *child_pop;
extern population *mixed_pop;
extern motif_t *motifs_data; // list of motifs
extern int n_motifs; // number of motifs that can be used
extern image_dataset_t image_dataset;


extern FILE **findividuals; // file to store the individuals
extern FILE *fobj; // file to store the objective function values during the simulation
extern FILE *fclass; // file to store the classification obtained for the samples
extern int currentGeneration;

/**
 *  allocate.c
 * Function related to memory allocation and deallocation of struct
 */
void allocate_memory_pop (NSGA2Type *nsga2Params,  population *pop, int size);
void allocate_memory_ind (NSGA2Type *nsga2Params, individual *ind);
void deallocate_memory_pop (NSGA2Type *nsga2Params, population *pop, int size);
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind);
void deallocate_memory_pop_snn_only (NSGA2Type *nsga2Params, population *pop, int size);
void deallocate_memory_snn_only (NSGA2Type *nsga2Params, individual *ind);
void allocate_int_array(int_array_t *int_array, int n);
void allocate_int_array_list(int_array_list_t *list_int_array, int n);
void deallocate_int_array(int_array_t *int_array);
void deallocate_int_arrays(int_array_t *int_arrays, int n);
void deallocate_int_array_list(int_array_list_t *list_int_array, int n);
void deallocate_sparse_matrix_node(sparse_matrix_node_t *sparse_matrix_node);

/**
 * 
 * 
 */
double maximum (double a, double b);
double minimum (double a, double b);
void print_double_matrix(double *matrix, int n);
void copy_motif_node(new_motif_t *motif1, new_motif_t *motif2);
void copy_motifs(individual *ind1, individual *ind2);
void copy_neuron_node(neuron_node_t *neuron_node1, neuron_node_t *neuron_node2);
void copy_neuron_nodes(individual *ind1, individual *ind2);
void copy_synapse_node(sparse_matrix_node_t *synapse1, sparse_matrix_node_t *synapse2);
void copy_synapse_nodes(individual *ind1, individual *ind2);
void copy_individual(individual *ind1, individual *ind2);
void copy_connectivity(individual *ind1, individual *ind2);
void print_individuals(NSGA2Type *nsga2Params, population *pop);
void print_networks(NSGA2Type *nsga2Params, population *pop);
void get_complete_matrix_from_dynamic_list(int *complete_matrix, sparse_matrix_node_t *matrix_node, int n_neurons);
void print_matrix_from_dynamic_list(sparse_matrix_node_t *matrix_node, int n_neurons);
void print_connectivity_matrix(int *matrix, int n);
void print_synapses_dynamic_list(sparse_matrix_node_t *matrix_node);
void load_individual_from_file(FILE *f, individual *ind);
void store_individual_in_file(FILE *f, individual *ind);


void crossover (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);
void realcross (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);
void bincross (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);

void assign_crowding_distance_list (NSGA2Type *nsga2Params,  population *pop, list *lst, int front_size);
void assign_crowding_distance_indices (NSGA2Type *nsga2Params,  population *pop, int c1, int c2);
void assign_crowding_distance (NSGA2Type *nsga2Params,  population *pop, int *dist, int **obj_array, int front_size);

/* Decodification */
void decode_pop (NSGA2Type *nsga2Params, population *pop);
void decode_ind (NSGA2Type *nsga2Params, individual *ind);

void onthefly_display (NSGA2Type *nsga2Params, population *pop, FILE *gp, int ii);

int check_dominance (NSGA2Type *nsga2Params, individual *a, individual *b);

void evaluate_pop (NSGA2Type *nsga2Params, population *pop, void *, void *);
void evaluate_ind (NSGA2Type *nsga2Params, individual *ind, void *inp, void *out, selected_samples_info_t *selected_samples_info);


void fill_nondominated_sort (NSGA2Type *nsga2Params,  population *mixed_pop, population *new_pop);
void crowding_fill (NSGA2Type *nsga2Params,  population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

/**
 * initialize.c
 * Functions for initialization TODO: I think some functions are not correctly located here. Probably those related to dynamic lists
 * management should be moved to another place
 */
// general initialization
void initialize_pop (NSGA2Type *nsga2Params, population *pop);
void initialize_ind (NSGA2Type *nsga2Params, individual *ind);
void initialize_ind_not_random (NSGA2Type *nsga2Params, individual *ind, int n_motifs, int *motif_types, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs);
void general_initialization(NSGA2Type *nsga2Params, individual *ind);
void general_initialization_not_random_motifs(NSGA2Type *nsga2Params, individual *ind, int n_motifs);
// motif nodes
void initialize_ind_motifs(individual *ind);
void initialize_ind_motifs_from_types(individual *ind, int *motif_types);
new_motif_t* initialize_and_allocate_motif(new_motif_t *motif_node, int motif_id, individual *ind);
void initialize_motif_node(new_motif_t *motif, int motif_id, individual *ind);
new_motif_t* initialize_and_allocate_motif_from_type(new_motif_t *motif_node, int motif_id, individual *ind, int type);
void initialize_motif_node_from_type(new_motif_t *motif, int motif_id, individual *ind, int type);
// neuron nodes
void initialize_neuron_nodes(NSGA2Type *nsga2Params, individual *ind);
neuron_node_t* initialize_and_allocate_neuron_node(NSGA2Type *nsga2Params, neuron_node_t *neuron_node);
void initialize_neuron_node(NSGA2Type *nsga2Params, neuron_node_t *neuron_node);
neuron_node_t* initialize_and_allocate_neuron_node_and_behaviour(NSGA2Type *nsga2Params, neuron_node_t *neuron_node, int behav);
void initialize_neuron_node_and_behaviour(NSGA2Type *nsga2Params, neuron_node_t *neuron_node, int behav);
void set_neurons_behaviour(individual *ind);
// connections: motifs x neurons & motifs x motifs
void connect_motifs_and_neurons(individual *ind);
// connect motifs
void connect_motifs(NSGA2Type *nsga2Params, individual *ind);
void connect_motifs_from_arrays(NSGA2Type *nsga2Params, individual *ind, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs);
void set_number_of_connections(individual *ind, int *n_connection_per_motif, int *n_connection_per_motif_done, int max_connect_per_motif);
void randomize_motif_connections(individual *ind, int *n_connections_per_motif);
void select_input_and_output_motifs_per_motif(individual *ind, int_array_t *selected_input_motifs_per_motif, int_array_t *selected_output_motifs_per_motif, int *n_input_connections_per_motif_done, int *n_output_connections_per_motif_done);
void initialize_lists_of_connectivity(individual *ind, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs);
// sparse matrix
void build_sparse_matrix(individual *ind, int_array_t *selected_input_motifs);
sparse_matrix_node_t* build_motif_sparse_matrix_columns(individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* build_neuron_sparse_matrix_column(individual *ind, sparse_matrix_node_t *synapse_node, int_array_t *selected_input_motifs, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* add_neuron_to_motif_connections(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* build_motif_internal_structure(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* build_connection(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* build_motif_internal_structure_and_connection(individual *ind, sparse_matrix_node_t *synapse_node, sparse_matrix_build_info_t *SMBI);
sparse_matrix_node_t* initialize_sparse_matrix_node(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col);
void initialize_sparse_matrix_node_only(individual *ind, sparse_matrix_node_t *matrix_node, int value, int row, int col);
// input synapses
void initialize_input_synapses(NSGA2Type *nsga2Params, individual *ind);
void initialize_synapse_weights(NSGA2Type *nsga2Params, individual *ind);
// int nodes
void initialize_int_node(int_node_t *int_node, int value, int_node_t *prev, int_node_t *next);
int_node_t* initialize_and_allocate_int_node(int value, int_node_t *prev, int_node_t *next);


void insert (list *node, int x);
list* del (list *node);

void merge(NSGA2Type *nsga2Params, population *pop1, population *pop2, population *pop3);
void copy_ind (NSGA2Type *nsga2Params, individual *ind1, individual *ind2);

/**
 * mutation.c 
 * Function related to individual mutations
 */
// general
void mutation_pop (NSGA2Type *nsga2Params, population *pop);
void mutation_ind (NSGA2Type *nsga2Params, individual *ind);
// neurons
void neuron_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code);
int_array_t* select_neurons_to_change(NSGA2Type *nsga2Params, individual *ind); // TODO: not here...
// synapses
void synapse_change_mutation(NSGA2Type *nsga2Params, individual *ind, int mutation_code);
int_array_t* select_synapses_to_change(NSGA2Type *nsga2Params, individual *ind); // TODO: not here...
// add motifs
void add_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int *new_motifs_types, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs);
void add_motifs_and_neurons_to_dynamic_lists(NSGA2Type *nsga2Params, individual *ind, int n_new_motifs, int *new_motifs_types);
void add_new_input_motifs_to_connectivity_lists(individual *ind, int_dynamic_list_t *connections, int_array_t *selected_motifs);
void update_sparse_matrix_add_motifs(individual *ind, int n_new_motifs, int_array_t *selected_input_motifs);
int_array_t* select_motifs(individual *ind, int n_motifs, int min_connected_motifs, int max_connected_motifs); // here...
int_array_t* select_motifs_to_connect_with_new_motifs(individual *ind, int n_motifs, int n_new_motifs, int min_connected_motifs, int max_connected_motifs); // TODO
// remove motifs
void remove_motif_mutation(NSGA2Type *nsga2Params, individual *ind, int n_remove_motifs);
void remove_selected_motifs_mutation(NSGA2Type *nsga2Params, individual *ind, int_array_t *selected_motifs_to_remove);
int_array_t* remove_motifs_from_dynamic_list(individual *ind, int_array_t *selected_motifs_to_remove);
int_array_t* select_motifs_to_be_removed(individual *ind, int n_motifs);
void update_sparse_matrix_remove_motifs(individual *ind, int_array_t *selected_motifs);
sparse_matrix_node_t* remove_all_motif_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, sparse_matrix_node_t *synapse_node);
sparse_matrix_node_t* remove_selected_synapses(individual *ind, sparse_matrix_build_info_t *SMBI, int_array_t *selected_motifs, sparse_matrix_node_t *synapse_node);
// helpers
int_array_t* map_IO_motifs_to_input(individual *ind, int n_new_motifs, int_array_t *selected_input_motifs, int_array_t *selected_output_motifs);
int_array_t *map_from_input_motifs_to_output(individual *ind, int n_motifs, int n_new_motifs, int max_connected_motifs, int_array_t *all_selected_input_motifs);
// not used
void bin_mutate_ind (NSGA2Type *nsga2Params, individual *ind);
void real_mutate_ind (NSGA2Type *nsga2Params, individual *ind);


void test_SNN(NSGA2Type *nsga2Params, individual *ind, selected_samples_info_t *selected_samples_info); 
void test_problem (double *xreal, double *xbin, int **gene, double *obj, double *constr);
void simulate_by_samples_enas(spiking_nn_t *snn, NSGA2Type *nsga2Params, individual *ind, simulation_results_t *results, int n_selected_samples, int *selected_sample_indexes, image_dataset_t *dataset);
void select_samples(selected_samples_info_t *selected_samples_info, int n_samples, int mode, int *percentages);

void assign_rank_and_crowding_distance (NSGA2Type *nsga2Params, population *new_pop);

void report_pop (NSGA2Type *nsga2Params, population *pop, FILE *fpt);
void report_feasible (NSGA2Type *nsga2Params, population *pop, FILE *fpt);
void report_ind (individual *ind, FILE *fpt);

void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
void quicksort_dist(population *pop, int *dist, int front_size);
void q_sort_dist(population *pop, int *dist, int left, int right);
void insertion_sort(int *arr, int n); 

void selection (NSGA2Type *nsga2Params, population *old_pop, population *new_pop);
individual* tournament (NSGA2Type *nsga2Params, individual *ind1, individual *ind2);

/**
 * nsga2.c
 */
NSGA2Type ReadParameters(int argc, char **argv);
void InitNSGA2(NSGA2Type *nsga2Params, void *, void *);
int NSGA2(NSGA2Type *nsga2Params, void *, void *);
void print_nsga2Params(NSGA2Type *nsga2Params);


/**
* my functions
*/
void initialize_FFE(motif_t *motif);
void initialize_FBE(motif_t *motif);
void initialize_FBI(motif_t *motif);
void initialize_RCE(motif_t *motif);
void initialize_RCI(motif_t *motif);
void initialize_LTI(motif_t *motif);
void initialize_CPG(motif_t *motif);
void deallocate_motifs_data();

void initialize_motif(motif_t *motif);
void initialize_motifs();
int check_if_neuron_is_input(int motif_type, int neuron_index);
int check_if_neuron_is_output(int motif_type, int neuron_index);


# endif
#endif
