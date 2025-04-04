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


void sample_simulation(){

}


void simulate(spiking_nn_t *snn, void (*input_step)(), void (*output_step)(), simulation_configuration_t *conf, simulation_results_t *results){
    // Network simulation
    
    int time_step = 0, i; 

    struct timespec start, end; // to measure simulation complete time
    struct timespec start_neurons, end_neurons; // to measure neurons imulation time
    struct timespec start_synapses, end_synapses; // to measure synapses simulation time

    results->elapsed_time = 0;
    results->elapsed_time_neurons = 0;
    results->elapsed_time_synapses = 0;
    // alloc memory for generated spikes

    if(conf->simulation_type == 0) // clock-based
    { 
        clock_gettime(CLOCK_MONOTONIC, &start);
        while(time_step < conf->time_steps)
        {

#ifdef DEBUG ////////////////////////////////////////////////
            printf("Processing time step %d\n", time_step);
#endif       ////////////////////////////////////////////////

            // process all neurons
            clock_gettime(CLOCK_MONOTONIC, &start_neurons);
            #pragma omp parallel num_threads(n_process)
            {
                #pragma omp for schedule(static, 10) private(i) 
                for(i=0; i<snn->n_neurons; i++)
                    input_step(snn, time_step, i, results->generated_spikes);

                #pragma omp for schedule(static, 10) private(i)
                for(i=0; i<snn->n_neurons; i++)
                    output_step(snn, time_step, i, results->generated_spikes);

                #pragma omp single
                {
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                }

#ifdef DEBUG //////////////////////////////////////////////////////////////
                printf("Processing stdp on time step %d\n", time_step);
#endif      ///////////////////////////////////////////////////////////////

                // process STDP
                clock_gettime(CLOCK_MONOTONIC, &start_synapses);

                if(conf->learn != 0) // if learn == 0, the execution is only simulating withouth changing the weights
                {
                    #pragma omp for schedule(static, 50) private(i)
                    for(i = 0; i<snn->n_synapses; i++)
                        snn->synapses[i].learning_rule(&(snn->synapses[i])); 
                }
            }
            
            clock_gettime(CLOCK_MONOTONIC, &end_synapses);

            // time step computed
            time_step++;
        
            // store neuron simulation and training rule simulation times
            results->elapsed_time_neurons += (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
            results->elapsed_time_synapses += (end_synapses.tv_sec - start_synapses.tv_sec) + (end_synapses.tv_nsec - start_synapses.tv_nsec) / 1e9;

            // Print info about the simulation
            if(time_step % 1000 == 0)
                printf("Time step: %d/%d\n", time_step, conf->time_steps);

#ifdef DEBUG //////////////////////////////////////////////////////////
            printf(" - Printing synapses weights: \n");
            for(i = 0; i<snn->n_synapses; i++)
                printf(" -- Synapse %d: %f\n", i, snn->synapses[i].w);

            printf("\n=======================================\n\n");
#endif      //////////////////////////////////////////////////////////////

        }
        clock_gettime(CLOCK_MONOTONIC, &end);
    }
    else // event-driven
    {
        //TODO
        clock_gettime(CLOCK_MONOTONIC, &start);
        clock_gettime(CLOCK_MONOTONIC, &end);
    }

    results->elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}