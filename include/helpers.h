#ifndef HELPERS_H
#define HELPERS_H

/// @brief Print matrix
/// @param matrix Matrix to be printed
/// @param rows Number of rows of the matrix
/// @param columns Number of columns of the matrix
void print_matrix( int *matrix, int rows, int columns);


/// @brief Print an array
/// @param array Array to be printed
/// @param length Number of elements of the array
void print_array(int *array, int length);


/// @brief Print matrix (of real numbers)
/// @param matrix Matrix to be printed
/// @param rows Number of rows of the matrix
/// @param columns Number of columns of the matrix
void print_matrix_f(float *matrix, int rows, int columns);


/// @brief Print an array (of real numbers)
/// @param array Array to be printed
/// @param length Number of elements of the array
void print_array_f(float *array, int length);


/// @brief Generate a random spike pattern
/// @param output_file_name File to store the generated spike train
/// @param time_steps Time steps of the input spike 
/// @param prob Probability to generate a spike on an step
void random_input_spike_train_generator(const char *output_file_name, int time_steps, int prob);

#endif