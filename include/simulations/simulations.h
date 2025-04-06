/// FUNCTIONS WITH SIMULATION TYPES
#include "snn_library.h"
#include "load_data.h"
#include "helpers.h"
#include "training_rules/stdp.h"

#include "neuron_models/lif_neuron.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


void sample_simulation();

// la mayoría de parámetros se pueden encapsular en un struct tipo "simulation parameters" o algo así
void simulate(spiking_nn_t *snn, simulation_configuration_t *conf, simulation_results_t *results);
