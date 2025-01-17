//#include "../include/snn_library.h"

void print_matrix(int rows, int columns, int *matrix){
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            printf("%d ", matrix[i*columns + j]);
        }
        printf("\n");
    }
}