/* Routine for evaluating population members  */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <time.h>

# include "nsga2.h"
# include "rand.h"

# include "snn_library.h"
# include "load_data.h"
# include "helpers.h"
# include "training_rules/stdp.h"
# include "neuron_models/lif_neuron.h"
# include "snnfuncs.h"
# include "distance_comp.h"

typedef struct
{
    int index; // index in tthe sample list, not in the global dataset list
    double mean_distance;
    int farthest_point_index;
    double farthest_point_distance;
} centroid_info_t;

/* Routine to evaluate objective function values and constraints for a population */
void evaluate_pop (NSGA2Type *nsga2Params, population *pop, void *inp, void *out)
{
    int i, j, mode, n_samples, n_repetitions;
    selected_samples_info_t *selected_samples_info;

    // load info from struct
    mode = nsga2Params->mode;
    n_samples = nsga2Params->n_samples;
    n_repetitions = nsga2Params->n_repetitions;

    // allocate memory to store information of selected samples for each repetition
    selected_samples_info = (selected_samples_info_t *)malloc(nsga2Params->n_repetitions * sizeof(selected_samples_info_t));
    for(i = 0; i<n_repetitions; i++){
        //printf(" > Selecting samples for repetition %d\n", i);
        //selected_samples_info[i].sample_indexes = (int *)malloc(n_samples * sizeof(int));
        //selected_samples_info[i].labels = (int *)calloc(n_samples, sizeof(int));
        //selected_samples_info[i].n_selected_samples_per_class = (int *)calloc(n_samples, sizeof(int));

        // select samples to simulate on this repetition
        select_samples(&(selected_samples_info[i]), n_samples, mode, NULL); 
    }

    for (i=0; i<nsga2Params->popsize; i++)
    {
        //printf(" > Evaluating individual %d...\n", i);
        evaluate_ind (nsga2Params, &(pop->ind[i]), inp, out, selected_samples_info);
        //printf(" > Individual %d evaluated!\n", i);
    }

    // free memory of selected samples
    for(i = 0; i<n_repetitions; i++){
        free(selected_samples_info[i].sample_indexes);
        free(selected_samples_info[i].labels);
        free(selected_samples_info[i].n_selected_samples_per_class);
        for(j = 0; j<image_dataset.n_classes; j++){
            free(selected_samples_info[i].sample_indexes_per_class[j]);
        }
        free(selected_samples_info[i].sample_indexes_per_class);
    }
    free(selected_samples_info);

    return;
}

/* Routine to evaluate objective function values and constraints for an individual */
void evaluate_ind (NSGA2Type *nsga2Params, individual *ind, void *inp, void *out, selected_samples_info_t *selected_samples_info)
{
    int i, j;
    
    test_SNN(nsga2Params, ind, selected_samples_info);

    //printf(" >> Obj 1 = %lf, obj 2 = %lf\n", ind->obj[0], ind->obj[1]);

    // provisional 
    
    //test_problem (ind->xreal, ind->xbin, ind->gene, ind->obj, ind->constr);
    
    if (nsga2Params->ncon==0)
    {
        ind->constr_violation = 0.0;
    }
    else
    {
        ind->constr_violation = 0.0;
        for (j=0; j<nsga2Params->ncon; j++)
        {
            if (ind->constr[j]<0.0)
            {
                ind->constr_violation += ind->constr[j];
            }
        }
    }

    return;
}


/* Problem for SNNs */
void test_SNN(NSGA2Type *nsga2Params, individual *ind, selected_samples_info_t *selected_samples_info){
    // simulate samples 
    int i, j, l, n_samples, n_neurons, n_classes, time_steps, n_repetitions, rep, mode, n_obj;
    int *sample_indexes, *labels, *n_selected_samples_per_class, **sample_indexes_per_class;
    int **spike_amount_per_neurons_per_sample, *max_distance_per_sample_index;
    double *distance_matrix, *mean_distance_per_sample, *max_distance_per_sample, *inter_class_distance_matrix, **obj;
    int temp_label, temp_mean, temp_global_index, temp_local_index, temp_global_index2, temp_local_index2;
    centroid_info_t *centroid_info;
    int n_inter_class_distances;

    // load info from struct
    mode = nsga2Params->mode;
    n_samples = nsga2Params->n_samples;
    n_repetitions = nsga2Params->n_repetitions;
    n_neurons = ind->snn->n_neurons;
    n_classes = image_dataset.n_classes; // TODO: Generalize this, now is only valid for this dataset
    time_steps = nsga2Params->bins;
    n_obj = nsga2Params->nobj;
    

    // Initialize struct for storing the results and the configuration of them
    simulation_results_t results;
    results_configuration_t conf;
    conf.n_samples = n_samples;
    conf.n_neurons = n_neurons;
    conf.time_steps = time_steps;

    // call function to allocate memory for results struct
    initialize_results_struct(&results, &conf);


    // TODO: this function should be more general for allowing the use of different objective functions

    // =================================== //
    // Allocate memory for neccessary data //
    // =================================== //

    // arrays to store the amount of spikes generated by each neuron during the simulation of a set of samples [n_samples x n_neurons]
    spike_amount_per_neurons_per_sample = (int **)malloc(n_samples * sizeof(int *));
    for(i=0; i<n_samples; i++)
        spike_amount_per_neurons_per_sample[i] = (int *)malloc(n_neurons * sizeof(int));

    // distance matrix to store distances between spike trains generated for different samples [n_samples x n_samples]
    distance_matrix = (double *)calloc(n_samples * n_samples, sizeof(double));
    

    // array to store information of clusters: the centroid of each class cluster, the mean distance from the centroid to the rest of samples, and the distance to the sample that is farther
    centroid_info = (centroid_info_t *)calloc(n_classes, sizeof(centroid_info_t));

    // temporal lists to store the mean and max distances to the rest of samples of the class for each sample, and the index of that sample in the list of SELECTED samples (not in the global list of samples) 
    mean_distance_per_sample = (double *)calloc(n_samples, sizeof(double));
    max_distance_per_sample_index = (int *)calloc(n_samples, sizeof(int));
    max_distance_per_sample = (double *)calloc(n_samples, sizeof(double));

    // matrix to store the distances between the classes (distances between the centroids of each class) [n_classes x n_classes]
    inter_class_distance_matrix = (double *)calloc(n_classes * n_classes, sizeof(double));


    // lists to store the values of the objective functions of different repetitions
    obj = (double **)calloc(nsga2Params->nobj, sizeof(double *));
    
    for(i = 0; i<nsga2Params->nobj; i++)
        obj[i] = (double *)calloc(n_repetitions, sizeof(double));



    // ========================================================= //
    // Simulate the networks and compute the objective functions //
    // ========================================================= //
    
    for(rep = 0; rep<n_repetitions; rep++){
        
        // get information about the samples that will be computed in this repetition 
        sample_indexes = selected_samples_info[rep].sample_indexes; // store the indexes of the selected samples
        labels = selected_samples_info[rep].labels; // store the labels of the selected samples
        n_selected_samples_per_class = selected_samples_info[rep].n_selected_samples_per_class; // store the number of samples selected per each class
        sample_indexes_per_class = selected_samples_info[rep].sample_indexes_per_class; // store the indexes of the samples for each class

        // reinitialize lists for this repetition
        for(i=0; i<n_samples; i++){
            mean_distance_per_sample[i] = 0;
            max_distance_per_sample_index[i] = 0;
            max_distance_per_sample[i] = 0;
        }

        // reinitialize results struct
        reinitialize_results_struct(&results, &conf);


        /*
        printf(" >>>> In repetition %d!\n", rep);

        printf(" >>>>>>>> Print samples labels: ");
        for(i = 0; i<n_samples; i++){
            printf("%d ", image_dataset.labels[sample_indexes[i]]);
        }
        printf("\n");

        printf(" >>>>>>>> Print number of samples per class: ");
        for(i = 0; i<image_dataset.n_classes; i++){
            printf("%d ", n_selected_samples_per_class[i]);
        }
        printf("\n");
        */


        // ================ //
        // Simulate network //
        // ================ //

        // initialize weights for this repetition randomly
        initialize_synapse_weights(nsga2Params, ind);

        // simulate the network // TODO: generalize dataset management
        simulate_by_samples_enas(ind->snn, nsga2Params, ind, &results, n_samples, sample_indexes, &image_dataset);



        // =========================== //
        // Compute objective functions //
        // =========================== //

        // TODO: First objective: probably should be moved to a function for Modularity

        // copy the amount of spikes from the results struct // TODO: Move this to a function
        for(i = 0; i<n_samples; i++)
            for(j = 0; j<ind->snn->n_neurons; j++)
                spike_amount_per_neurons_per_sample[i][j] = results.results_per_sample[i].n_spikes_per_neuron[j];


        // TODO: This should be moved to a function and gneeralized to allow the use of more distance metrics
        // compute the distance matrix for this repetition // Paralelize????
        for(i = 0; i<n_samples - 1; i++)
            for(j = i + 1; j<n_samples; j++)
                distance_matrix[i * n_samples + j] = 
                    compute_manhattan_distance(spike_amount_per_neurons_per_sample[i], spike_amount_per_neurons_per_sample[j], n_neurons);//nsga2Params->bins);
        
        //print_double_matrix(distance_matrix, n_samples);


        // Find centroids and fill neccessary data of "clusters"
        // First reinitialize values
        for(i = 0; i<image_dataset.n_classes; i++){
            centroid_info[i].mean_distance = 9999999999999;
            centroid_info[i].farthest_point_distance = 0;
        }

        // TODO: This should be moved to a function
        // compute the mean distances to the rest of samples of the class for each class and the max distances [Computaitonal Cost n_samples * (n_samples / n_classes - 1)]
        for(i = 0; i<image_dataset.n_classes; i++){
            for(j=0; j<n_selected_samples_per_class[i]; j++){
                temp_local_index = sample_indexes_per_class[i][j]; // get the index of the sample in the list of samples
                temp_global_index = sample_indexes[temp_local_index]; // get the global index of the sample

                // compute the mean distance with the rest of elements of the class
                for(l=j+1; l<n_selected_samples_per_class[i]; l++){
                    temp_local_index2 = sample_indexes_per_class[i][l]; // get the index of the sample in the list of samples
                    temp_global_index2 = sample_indexes[temp_local_index2]; // get the global index of the sample

                    // sum distances
                    mean_distance_per_sample[temp_local_index] += distance_matrix[temp_local_index * n_samples + temp_local_index2];
                    if(distance_matrix[temp_local_index * n_samples + temp_local_index2] > max_distance_per_sample[temp_local_index]){
                        max_distance_per_sample_index[temp_local_index] = temp_local_index2;
                        max_distance_per_sample[temp_local_index] = distance_matrix[temp_local_index * n_samples + temp_local_index2];
                    }

                    mean_distance_per_sample[temp_local_index2] += distance_matrix[temp_local_index * n_samples + temp_local_index2];
                    if(distance_matrix[temp_local_index * n_samples + temp_local_index2] > max_distance_per_sample[temp_local_index2]){
                        max_distance_per_sample_index[temp_local_index2] = temp_local_index;
                        max_distance_per_sample[temp_local_index2] = distance_matrix[temp_local_index * n_samples + temp_local_index2];
                    }
                }

                // compute the mean for the actual element
                mean_distance_per_sample[temp_local_index] = mean_distance_per_sample[temp_local_index] / n_selected_samples_per_class[i];

                // store the information if the mean distance is smaller
                if(mean_distance_per_sample[temp_local_index] < centroid_info[i].mean_distance){
                    centroid_info[i].mean_distance = mean_distance_per_sample[temp_local_index];
                    centroid_info[i].index = temp_local_index;

                    centroid_info[i].farthest_point_index = max_distance_per_sample_index[temp_local_index];
                    centroid_info[i].farthest_point_distance = max_distance_per_sample[temp_local_index];

                    //printf(" >>>>>> Class %d, mean internal distance %lf, max distance %lf\n", i, centroid_info[i].mean_distance, centroid_info[i].farthest_point_distance);
                }   
            }
        }

        //printf("\n");
        // compute the distances between the centroids of each class // TODO: I am using a full matrix, but this should be simplified to only the upper triangle
        n_inter_class_distances = 0;
        for(i = 0; i<image_dataset.n_classes-1; i++){
            for(j = i+1; j<image_dataset.n_classes; j++){
                // get distance between class i and class j centroid from distance matrix
                temp_local_index = centroid_info[i].index;
                temp_local_index2 = centroid_info[j].index;

                // get distance between both class centers
                if(temp_local_index < temp_local_index2)
                    inter_class_distance_matrix[i * image_dataset.n_classes + j] = distance_matrix[temp_local_index * n_samples + temp_local_index2];
                else
                    inter_class_distance_matrix[i * image_dataset.n_classes + j] = distance_matrix[temp_local_index2 * n_samples + temp_local_index];

                //printf(" >>>>>>> Distance between %d and %d class centroids: %lf\n", i, j, inter_class_distance_matrix[i * image_dataset.n_classes + j]);
                n_inter_class_distances ++;
            }
        }

        // Now we have all the distances we need to compute the objective function

        for(i=0; i<image_dataset.n_classes-1; i++){
            for(j=i+1; j<image_dataset.n_classes; j++){
                obj[0][rep] += 
                    (inter_class_distance_matrix[i * image_dataset.n_classes + j] / 
                    (maximum(centroid_info[i].farthest_point_distance, centroid_info[j].farthest_point_distance) + inter_class_distance_matrix[i * image_dataset.n_classes + j]));
            }
        }
        obj[0][rep] /= n_inter_class_distances;        

        // TODO: Second objective: probably should be moved to a function for Modularity
        for(i = 0; i<n_samples; i++){
            for(j = 0; j<ind->snn->n_neurons; j++){
                obj[1][rep] += spike_amount_per_neurons_per_sample[i][j];
            }
        }
        // compute the mean for all samples
        obj[1][rep] /= n_samples;
    }



    // =========================================== //
    // compute the mean of the objective functions //
    // =========================================== //

    for(i = 0; i<n_obj; i++){
        ind->obj[i] = 0;
        for(j = 0; j<n_repetitions; j++){
            ind->obj[i]+=obj[i][j];
        }
        ind->obj[i] /= n_repetitions;
    }



    // ======================== //
    // free all the memory used //
    // ======================== //

    for(i = 0; i<n_obj; i++)
        free(obj[i]);
    free(obj);
    free(inter_class_distance_matrix);
    free(max_distance_per_sample);
    free(mean_distance_per_sample);
    free(max_distance_per_sample_index);
    free(centroid_info);
    free(distance_matrix);

    for(i=0; i<n_samples; i++)
        free(spike_amount_per_neurons_per_sample[i]);
    free(spike_amount_per_neurons_per_sample);

    free_results_struct_memory(&results, &conf);

    return;
}


// The way to deal with datasets should be revised and generalized
void simulate_by_samples_enas(spiking_nn_t *snn, NSGA2Type *nsga2Params, individual *ind, simulation_results_t *results, int n_selected_samples, int *selected_sample_indexes, image_dataset_t *dataset){
    int time_step = 0, i, j, l; 
    int n_process = nsga2Params->n_process;
    struct timespec start, end, start_bin, end_bin; // to measure simulation complete time
    struct timespec start_neurons, end_neurons; // to measure neurons imulation time
    struct timespec start_synapses, end_synapses; // to measure synapses simulation time
    struct timespec start_neurons_input, end_neurons_input; // to measure synapses simulation time
    struct timespec start_neurons_output, end_neurons_output; // to measure synapses simulation time

    double elapsed_time;

    synapse_t *synap;
    simulation_results_per_sample_t *results_per_sample;

    // loop over selected samples
    for(i = 0; i<n_selected_samples; i++){
        //printf(" >>>>>>>> In sample %d\n", i);
        // get results struct
        results_per_sample = &(results->results_per_sample[i]);

        //printf(" >> Starting processing sample %d...\n", i);
        // initialize network (THIS MUST BE MOVED TO A FUNCTION AND GENERALIZED FOR DIFFERENT NEURON TYPES
        clock_gettime(CLOCK_MONOTONIC, &start);

        /*#pragma omp parallel for schedule(dynamic, 10) private(j)  // this and the next loops: 4.5 seconds
        for(j = 0; j<snn->n_neurons; j++){
            reinitialize_LIF_neurons_from_genotype(snn, j);
        }*/
        reinitialize_LIF_neurons_from_genotype(snn, ind);
        //printf(" >>>>>>>>>>>>> Neurons reinitialized!\n");


        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        //printf(" >> Neurons reinitialized in %f seconds!\n", elapsed_time);
        
        
        // reinitialize synapses
        clock_gettime(CLOCK_MONOTONIC, &start);

        #pragma omp parallel for schedule(dynamic, 10) private(j) 
        for(j=0; j<snn->n_synapses; j++){
            reinitialize_synapse_from_genotype(snn, j);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        //printf(" >>>>>>>>>>>>> Synapses reinitialized!\n");


        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        //printf(" >> Synapses reinitialized in %f seconds!\n", elapsed_time);

        // introduce spikes into input neurons input synapses (I think that this is not paralelizable)
        // try using pointers directly
        clock_gettime(CLOCK_MONOTONIC, &start);

        // this inserts the input spikes in the first neurons first synapses
        for(j=0; j<snn->n_input; j++){ // 3 seconds??
            synap = &(snn->synapses[snn->lif_neurons[j].input_synapse_indexes[0]]);
            
            for(l = 0; l<dataset->images[i].image[j][0]; l++){
                synap->l_spike_times[synap->last_spike] = dataset->images[i].image[j][l];
                synap->last_spike +=1;
            }
        }
        //printf(" >>>>>>>>>>>>> Input introduced!\n");

        clock_gettime(CLOCK_MONOTONIC, &end);

        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;        
        //printf(" >> Spikes introduced on input neurons in %f seconds!\n", elapsed_time);

        // check that the image has been correctly loaded into the neuron synapse
        /*for(l = 0; l<2; l++){
            if(dataset->images[i].image[l][0]>0){
                for(j=0; j<dataset->images[i].image[l][0]; j++){
                    printf("(%d, %d) ", dataset->images[i].image[l][j], snn->synapses[snn->lif_neurons[l].input_synapse_indexes[0]].l_spike_times[j]);
                }
                printf("\n");
            }
        }*/

        clock_gettime(CLOCK_MONOTONIC, &start);



        // run the simulation
        for(j=0; j<dataset->bins * 10; j++){ 
            //printf(" >>>>>>>>>>>>> In bin %d\n", j);
            clock_gettime(CLOCK_MONOTONIC, &start_bin);

            // process training sample
            //printf("processing bin %d\n", j);
            #pragma omp parallel num_threads(n_process)
            {
                clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                #pragma omp for schedule(dynamic, 10) private(l) 
                for(l=0; l<snn->n_neurons; l++){
                    snn->input_step(snn, j, l, results_per_sample);
                } 
                clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                results_per_sample->elapsed_time_neurons_input = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                //printf(" >> Input neurons: %f seconds!\n", results_per_sample->elapsed_time_neurons_input);

                //printf("Input synapses processed\n");

                clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                #pragma omp for schedule(dynamic, 10) private(l)
                for(l=0; l<snn->n_neurons; l++){
                    snn->output_step(snn, j, l, results_per_sample);
                }
                clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                results_per_sample->elapsed_time_neurons_output = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                //printf(" >> Output neurons: %f seconds!\n", results_per_sample->elapsed_time_neurons_output);

                //printf("Output synapses processed\n");
                // stdp

                /*clock_gettime(CLOCK_MONOTONIC, &start_neurons);
                #pragma omp for schedule(dynamic, 50) private(l)
                for(l  = 0; l<snn->n_synapses; l++){
                    snn->synapses[l].learning_rule(&(snn.synapses[l])); 
                }
                clock_gettime(CLOCK_MONOTONIC, &end_neurons);
                synapses_elapse_time = (end_neurons.tv_sec - start_neurons.tv_sec) + (end_neurons.tv_nsec - start_neurons.tv_nsec) / 1e9;
                printf(" >> Synapses: %f seconds\n", synapses_elapse_time);
                */

                //printf("Training rules processed\n");
                // store generated output into another variable


                // train?
                clock_gettime(CLOCK_MONOTONIC, &end_bin);
                results_per_sample->elapsed_time_neurons = (end_bin.tv_sec - start_bin.tv_sec) + (end_bin.tv_nsec - start_bin.tv_nsec) / 1e9;
                //printf(" >> Bin simulation: %f seconds\n", results_per_sample->elapsed_time_neurons);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        results_per_sample->elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        //printf(" >> Sample simulation: %f seconds\n", results_per_sample->elapsed_time_neurons  );
    }
}

// TODO: document this function and add mode 2
void select_samples(selected_samples_info_t *selected_samples_info, int n_samples, int mode, int *percentages){
    int i, j, temp_sample_index, class, valid, n_samples_per_class, temp_label;

    // allocate memory
    selected_samples_info->sample_indexes = (int *)malloc(n_samples * sizeof(int));
    selected_samples_info->labels = (int *)calloc(n_samples, sizeof(int));
    selected_samples_info->n_selected_samples_per_class = (int *)calloc(image_dataset.n_classes, sizeof(int));
    selected_samples_info->sample_indexes_per_class = (int **)calloc(image_dataset.n_classes, sizeof(int *));


    for(i=0; i<n_samples; i++){
        selected_samples_info->sample_indexes[i] = 0; 
    }

    for(i=0; i<image_dataset.n_classes; i++){
        selected_samples_info->n_selected_samples_per_class[i] = 0;
    }

    // in case amount of samples per class should be balanced, calculate the number of samples for each class
    if(n_samples % image_dataset.n_classes == 0)
        n_samples_per_class = n_samples / image_dataset.n_classes;
    else
        n_samples_per_class = (int)(n_samples / image_dataset.n_classes) + 1;


    // select samples
    for(i = 0; i<n_samples; i++){
        valid = 0;

        // loop until a sample that is not previously selected is selected
        while(valid == 0){
            valid = 1;

            temp_sample_index = rand() % image_dataset.n_images;
            
            // check if sample is already selected
            for(j = 0; j<i; j++){
                // if the sample has been selected before, not valid
                if(selected_samples_info->sample_indexes[j] == temp_sample_index){
                    valid = 0;
                }
            }

            // if mode == 1, selected samples classes must be balanced
            if(mode == 1){
                // get selected sample label
                temp_label = image_dataset.labels[temp_sample_index];

                // check if we can select more samples of this class
                if(selected_samples_info->n_selected_samples_per_class[temp_label] >= n_samples_per_class)
                    valid = 0;
            }
        }

        // store the selected sample and the label
        selected_samples_info->sample_indexes[i] = temp_sample_index;
        selected_samples_info->n_selected_samples_per_class[temp_label] ++;
        selected_samples_info->labels[i] = temp_label;
    }

    // allocate memory for each class selected samples indexes
    int *next_index_class = (int *)calloc(image_dataset.n_classes, sizeof(int));
    int temp_class;
    for(i = 0; i<image_dataset.n_classes; i++)
        selected_samples_info->sample_indexes_per_class[i] = (int *)malloc(selected_samples_info->n_selected_samples_per_class[i] * sizeof(int));

    // loop over all selected samples and add each one to the corresponding list
    for(i = 0; i<n_samples; i++){
        // add the selected i.th sample index to the corresponding class 
        temp_class = selected_samples_info->labels[i];
        selected_samples_info->sample_indexes_per_class[temp_class][next_index_class[temp_class]] = i;
        next_index_class[temp_class] ++; // next index 
    }


    free(next_index_class);
}