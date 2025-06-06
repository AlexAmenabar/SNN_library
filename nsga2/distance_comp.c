# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "rand.h"

# include "distance_comp.h"

/* Function to compute the manhattan distance for two arrays*/
int compute_manhattan_distance(int *array1, int *array2, int n){
    int i, distance = 0;
    
    for(i = 0; i<n; i++){
        distance += abs(array1[i] - array2[i]);
    }

    return distance;
}