//#include "../include/snn_library.h"

void print_matrix(int rows, int columns, int *matrix){
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

void print_matrix_f(int rows, int columns, float *matrix){
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