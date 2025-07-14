# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <unistd.h>

void main(int argc, int *argv[]){

    int *prueba = (int *)malloc(0);

    printf("Allocated\n");
    fflush(stdout);

    free(prueba);
    printf("Deallocated\n");
    fflush(stdout);
}