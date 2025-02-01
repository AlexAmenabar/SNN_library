#include "helpers.h"

void print_matrix( int *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%d ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array(int *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
}

void print_matrix_f(float *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%f ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array_f(float *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%f ", array[i]);
    }
    printf("\n");
}

void random_input_spike_train_generator(const char *output_file_name, int time_steps, int prob){
    for(int i = 0; i<time_steps; i++){
        int p = rand() % 100;
        if(p > prob){
            // generate spike on i time step
        }
    }
}