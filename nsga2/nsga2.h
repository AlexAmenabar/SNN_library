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

# include "snn_library.h"


# define INF 1.0e14
# define EPS 1.0e-14
# define eul  2.71828182845905
# define pi 3.14159265358979
# define GNUPLOT_COMMAND "gnuplot -persist"

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


/* Motifs */
typedef struct
{
    uint motif_id; // can not be negative --> can be the index???
    uint8_t neuron_index; // can not be negative, and it is local inside motif, so can not be a large number
                          // and it is a input neuron of the motif, it is stored the index of the input neuron
}
neuron_identifier_t;

typedef struct
{
    uint motif_id; 
    uint8_t motif_type;
    uint32_t initial_global_index; // index of the first neuron in the neuron list


    // I am not being able to do it with the info below, so I will use a sparse matrix

    // for each output neuron, the neurons to which is connected
    int *n_connections_to_motifs; // number of motifs each output neuron is connected to
    int *n_input_synapses_per_neuron; // number of input connections per each input neuron
    int *n_output_synapses_per_neuron; // number of output connections per each output neuron

    // these lists are used to manage the creation of new motifs and connections on crossover and mutation operations
    neuron_identifier_t **output_neurons; // list of postsinaptic neurons identifiers (tuple of motif id and neuron index)
    neuron_identifier_t **input_neurons; // list of presinaptic neurons identifier (tuple of motif id and neuron index)
    // this could be a triple pointer too and locate outside pf here, it would be a better place
}
motif_node_t;

typedef struct neuron_node_t
{
    double threshold;
    uint8_t refract_time;
    double v_rest;
    uint8_t r;

    // next neuron
    struct neuron_node_t *next_neuron;
}
neuron_node_t;

typedef struct sparse_matrix_node_t
{
    uint32_t row, col;
    int8_t value;

    uint8_t latency; //latency of the synaptic connection
    double weight;
    uint8_t learning_rule;

    // next element of the sparse matrix
    struct sparse_matrix_node_t *next_element;
}
sparse_matrix_node_t;


// change individual class to be a snn
typedef struct
{
    int n_motifs; // number of motifs for the individual
    motif_node_t *motifs;

    // dynamic list of neurons
    neuron_node_t *neurons;

    // sparse matrix of connections (dynamic list)
    sparse_matrix_node_t *connectivity_matrix; // connectivity sparse matrix: first pointer for rows, second one for columns, and the third one for values. It is a ordered list
    // int8_t as there can be negative and positive values (sign indicates if the weight must be positive or negative) and the value indicates the amount of connections between two neurons
}
genotype;

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
    genotype genotype; // genotype is a list of integers
    
    int n_motifs; // number of motifs for the individual
    motif_node_t *motifs;

    // network number of motifs, neurons and synapses
    int n_neurons; // total amount of neurons for the network

    int n_synapses; // total amount of synapses for the network

    // dynamic list of neurons
    neuron_node_t *neurons;

    // sparse matrix of connections (dynamic list)
    sparse_matrix_node_t *connectivity_matrix; // connectivity sparse matrix: first pointer for rows, second one for columns, and the third one for values. It is a ordered list
    // int8_t as there can be negative and positive values (sign indicates if the weight must be positive or negative) and the value indicates the amount of connections between two neurons 
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


typedef struct NSGA2Type
{
    // general variables
    double seed; // seed to randomize execution
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

    // dataset variables
    int dataset_type; // for the future, now only images
    int train_samples;
    int test_samples;
    int image_size;
    int bins;
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
extern motif_t *motifs; // list of motifs
extern int n_motifs; // number of motifs that can be used

/**
 *  allocate.c
 */
void allocate_memory_pop (NSGA2Type *nsga2Params,  population *pop, int size);
void allocate_memory_ind (NSGA2Type *nsga2Params, individual *ind);
void deallocate_memory_pop (NSGA2Type *nsga2Params, population *pop, int size);
void deallocate_memory_ind (NSGA2Type *nsga2Params, individual *ind);

double maximum (double a, double b);
double minimum (double a, double b);

void crossover (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);
void realcross (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);
void bincross (NSGA2Type *nsga2Params, individual *parent1, individual *parent2, individual *child1, individual *child2);

void assign_crowding_distance_list (NSGA2Type *nsga2Params,  population *pop, list *lst, int front_size);
void assign_crowding_distance_indices (NSGA2Type *nsga2Params,  population *pop, int c1, int c2);
void assign_crowding_distance (NSGA2Type *nsga2Params,  population *pop, int *dist, int **obj_array, int front_size);

void decode_pop (NSGA2Type *nsga2Params, population *pop);
void decode_ind (NSGA2Type *nsga2Params, individual *ind);

void onthefly_display (NSGA2Type *nsga2Params, population *pop, FILE *gp, int ii);

int check_dominance (NSGA2Type *nsga2Params, individual *a, individual *b);

void evaluate_pop (NSGA2Type *nsga2Params, population *pop, void *, void *);
void evaluate_ind (NSGA2Type *nsga2Params, individual *ind, void *, void *);


void fill_nondominated_sort (NSGA2Type *nsga2Params,  population *mixed_pop, population *new_pop);
void crowding_fill (NSGA2Type *nsga2Params,  population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

void initialize_pop (NSGA2Type *nsga2Params, population *pop);
void initialize_ind (NSGA2Type *nsga2Params, individual *ind);

void insert (list *node, int x);
list* del (list *node);

void merge(NSGA2Type *nsga2Params, population *pop1, population *pop2, population *pop3);
void copy_ind (NSGA2Type *nsga2Params, individual *ind1, individual *ind2);

void mutation_pop (NSGA2Type *nsga2Params, population *pop);
void mutation_ind (NSGA2Type *nsga2Params, individual *ind);
void bin_mutate_ind (NSGA2Type *nsga2Params, individual *ind);
void real_mutate_ind (NSGA2Type *nsga2Params, individual *ind);

void test_problem (double *xreal, double *xbin, int **gene, double *obj, double *constr);

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

void initialize_motif(motif_t *motif);
void initialize_motifs(NSGA2Type *nsga2Params, void *, void *);

# endif
#endif
