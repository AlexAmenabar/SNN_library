#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
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

void print_matrix_uint8(__uint8_t *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%d ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array_uint8(__uint8_t *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
}


void print_matrix_f(double *matrix, int rows, int columns){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%f ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}

void print_array_f(double *array, int length){
    int i;
    for(i=0; i<length; i++){
        printf("%f ", array[i]);
    }
    printf("\n");
}

void random_input_spike_train_generator(int **spike_trains, int *n_spikes, int n_input_spike_trains, int time_steps, int prob){
    int p;

    // generate n input spike trains spike train
    for(int i = 0; i<n_input_spike_trains; i++){
        n_spikes[i] = 0;

        // generate spikes per each train
        for(int t=0; t<time_steps; t++){
            p = rand() % 100;

            // generate spike on time t
            if(p > prob){
                spike_trains[i][n_spikes[i]] = t;
                n_spikes[i] += 1;
            }
        }
    }
}