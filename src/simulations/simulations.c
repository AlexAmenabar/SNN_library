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


// The way to deal with datasets should be revised and generalized
/*void simulate_by_samples(spiking_nn_t *snn, simulation_configuration_t *conf, simulation_results_t *results, image_dataset_t *dataset){
    int time_step = 0, i, j, l; 
    int n_process = conf->n_process;
    struct timespec start, end; // to measure simulation complete time
    struct timespec start_neurons, end_neurons; // to measure neurons imulation time
    struct timespec start_synapses, end_synapses; // to measure synapses simulation time
    struct timespec start_neurons_input, end_neurons_input; // to measure synapses simulation time
    struct timespec start_neurons_output, end_neurons_output; // to measure synapses simulation time


    if(conf->simulation_type == 0){ // clock-based

        // loop over samples
        for(i = 0; i<dataset->n_images; i++){
            printf("Starting processing sample %d...\n", i);
            // initialize network (THIS MUST BE MOVED TO A FUNCTION AND GENERALIZED FOR DIFFERENT NEURON TYPES
            clock_gettime(CLOCK_MONOTONIC, &start);

            #pragma omp parallel for schedule(dynamic, 10) private(j)  // this and the next loops: 4.5 seconds
            for(j = 0; j<snn->n_neurons; j++){
                re_initialize_lif_neuron(snn, j);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Neurons initialization: %f seconds\n", elapsed_time);

            printf("Neuron reinitialized\n");
            // reinitialize synapses
            clock_gettime(CLOCK_MONOTONIC, &start);

            #pragma omp parallel for schedule(dynamic, 10) private(j) 
            for(j=0; j<snn->n_synapses; j++){
                re_initialize_synapse(&(snn->synapses[j]));
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Synapses initialization: %f seconds\n", elapsed_time);
            printf("Synapses reinitialized\n");

            // introduce spikes into input neurons input synapses (I think that this is not paralelizable)
            // try using pointers directly
            clock_gettime(CLOCK_MONOTONIC, &start);

            for(j=0; j<snn->n_input; j++){ // 3 seconds??
                
                //for(int s = 0; s<snn.lif_neurons[j].n_input_synapse; s++){
                    synapse_t *synap = &(snn.synapses[snn.lif_neurons[j].input_synapse_indexes[0]]);
                    
                    /*
                    synap->l_spike_times = spike_images.spike_images[i].spike_image[j];
                    synap->last_spike += spike_images.spike_images[i].spike_image[j][0];
                    if(synap->last_spike>0)
                        synap->next_spike = 1; // when copying the pointer the first position is the number of spikes
                    synap->max_spikes = synap->last_spike;
                    */
                    ///*
/*                    for(l = 0; l<dataset.images[i].image[j][0]; l++){
                        synap->l_spike_times[synap->last_spike] = spike_images.spike_images[i].spike_image[j][l];
                        synap->last_spike +=1;
                    }
                    //synap->max_spikes = synap->last_spike;
                    //
                //}
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("SPikes introduction into network: %f seconds\n", elapsed_time);
            
            printf("Spikes introduced on input neurons\n");

            // check that the image has been correctly loaded into the neuron synapse
            for(l = 0; l<784; l++){
                if(spike_images.spike_images[i].spike_image[l][0]>0){
                    for(j=0; j<spike_images.spike_images[i].spike_image[l][0]; j++){
                        printf("(%d, %d) ", spike_images.spike_images[i].spike_image[l][j], snn.synapses[snn.lif_neurons[l].input_synapse_indexes[0]].l_spike_times[j]);
                    }
                    printf("\n");
                }
            }

            clock_gettime(CLOCK_MONOTONIC, &start);

            for(j=0; j<dataset->bins; j++){ // this loop more or less 6 

                clock_gettime(CLOCK_MONOTONIC, &start_bin);

                // process training sample
                //printf("processing bin %d\n", j);
                #pragma omp parallel num_threads(n_process)
                {
                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 10) private(l) 
                    for(l=0; l<snn->n_neurons; l++){
                        input_step(snn, j, l, generated_spikes);
                    } 
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    input_neurons_elapsed_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Input neurons: %f seconds\n", input_neurons_elapsed_time);

                    //printf("Input synapses processed\n");

                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 10) private(l)
                    for(l=0; l<snn->n_neurons; l++){
                        output_step(snn, j, l, generated_spikes);
                    }
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    output_neurons_elapsed_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Output neurons: %f seconds\n", output_neurons_elapsed_time);

                    //printf("Output synapses processed\n");
                    // stdp

                    clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                    #pragma omp for schedule(dynamic, 50) private(l)
                    for(l  = 0; l<snn->n_synapses; l++){
                        snn->synapses[l].learning_rule(&(snn.synapses[l])); 
                    }
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    synapses_elapse_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                    printf(" - Synapses: %f seconds\n", synapses_elapse_time);

                    //printf("Training rules processed\n");
                    // store generated output into another variable


                    // train?
                    clock_gettime(CLOCK_MONOTONIC, &end_bin);
                    elapsed_time = (end_bin.tv_sec - start_bin.tv_sec) + (end_bin.tv_nsec - start_bin.tv_nsec) / 1e9;
                    printf("Bin simulation: %f seconds\n", elapsed_time);
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Sample simulation: %f seconds\n", elapsed_time);
        }
    }
    else{ // event-driven

    }
}*/


void simulate(spiking_nn_t *snn, simulation_configuration_t *conf, simulation_results_t *results){
    // Network simulation
    
    int time_step = 0, i; 
    int n_process = conf->n_process;
    struct timespec start, end; // to measure simulation complete time
    struct timespec start_neurons, end_neurons; // to measure neurons imulation time
    struct timespec start_synapses, end_synapses; // to measure synapses simulation time
    struct timespec start_neurons_input, end_neurons_input; // to measure synapses simulation time
    struct timespec start_neurons_output, end_neurons_output; // to measure synapses simulation time

    // there is only one sample
    simulation_results_per_sample_t *results_per_sample = &(results->results_per_sample[0]);


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

#ifdef OPENMP
                #pragma omp single
                {
                clock_gettime(CLOCK_MONOTONIC, &start_neurons_input);
                }
                #pragma omp for schedule(static, 10) private(i) 
                for(i=0; i<snn->n_neurons; i++)
                    snn->input_step(snn, time_step, i, results_per_sample);
                #pragma omp single
                {
                clock_gettime(CLOCK_MONOTONIC, &end_neurons_input);
                }


                #pragma omp single
                {
                clock_gettime(CLOCK_MONOTONIC, &start_neurons_output);
                }
                
                #pragma omp for schedule(static, 10) private(i)
                for(i=0; i<snn->n_neurons; i++)
                    snn->output_step(snn, time_step, i, results_per_sample);
                #pragma omp single
                {
                clock_gettime(CLOCK_MONOTONIC, &end_neurons_output);
                }

#else // if is serial compute input and neuron synapses at the same iteration 
                for(i = 0; i<snn->n_neurons; i++)
                    snn->complete_step(snn, time_step, i, results_per_sample);
#endif

                #pragma omp single
                {
                    clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                    clock_gettime(CLOCK_MONOTONIC, &start_synapses);
                }

#ifdef DEBUG //////////////////////////////////////////////////////////////
                printf("Processing stdp on time step %d\n", time_step);
#endif      ///////////////////////////////////////////////////////////////

                // process STDP

                //if(conf->learn != 0) // if learn == 0, the execution is only simulating withouth changing the weights
                //{
               
#ifndef NOLEARN // si se ha definido esto en compilación la red no aplicará el STDP o ninguna otra regla de aprendizaje
                #pragma omp for schedule(static, 50) private(i)
                for(i = 0; i<snn->n_synapses; i++)
                    snn->synapses[i].learning_rule(&(snn->synapses[i])); 
                //}
#endif
            }
            
            clock_gettime(CLOCK_MONOTONIC, &end_synapses);

            // time step computed
            time_step++;
        
            // store neuron simulation and training rule simulation times
            results_per_sample->elapsed_time_neurons += (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
            results_per_sample->elapsed_time_synapses += (end_synapses.tv_sec - start_synapses.tv_sec) + (end_synapses.tv_nsec - start_synapses.tv_nsec) / 1e9;
            results_per_sample->elapsed_time_neurons_input += (end_neurons_input.tv_sec - start_neurons_input.tv_sec) + (end_neurons_input.tv_nsec - start_neurons_input.tv_nsec) / 1e9;
            results_per_sample->elapsed_time_neurons_output += (end_neurons_output.tv_sec - start_neurons_output.tv_sec) + (end_neurons_output.tv_nsec - start_neurons_output.tv_nsec) / 1e9;

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

    results_per_sample->elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}