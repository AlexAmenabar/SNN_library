#ifndef HELPERS_H
#define HELPERS_H
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/**
Helper functions not related only to SNN use cases
*/

/// @brief Print matrix
/// @param matrix Matrix to be printed
/// @param rows Number of rows of the matrix
/// @param columns Number of columns of the matrix
void print_matrix( int *matrix, int rows, int columns);


/// @brief Print an array
/// @param array Array to be printed
/// @param length Number of elements of the array
void print_array(int *array, int length);

void print_matrix_uint8(__uint8_t *matrix, int rows, int columns);

void print_array_uint8(__uint8_t *array, int length);


/// @brief Print matrix (of real numbers)
/// @param matrix Matrix to be printed
/// @param rows Number of rows of the matrix
/// @param columns Number of columns of the matrix
void print_matrix_f(double *matrix, int rows, int columns);


/// @brief Print an array (of real numbers)
/// @param array Array to be printed
/// @param length Number of elements of the array
void print_array_f(double *array, int length);


/// @brief This function generates random spike trains
/// @param spike_trains List of lists to store the spike trains
/// @param n_spikes Pointer to store the number of spikes for each spike train
/// @param n_input_spike_trains Number of spike trains to be generated
/// @param time_steps Maximum number of spikes for each spike train 
/// @param prob Probability to generate a spike in a time step
void random_input_spike_train_generator(int **spike_trains, int *n_spikes, int n_input_spike_trains, int time_steps, int prob);

#endif