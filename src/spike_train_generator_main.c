#include "snn_library.h"
#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    
    srand(time(NULL)); // compilar

    int n_input_spike_trains, time_steps, prob;

    int **spike_trains, *n_spikes;

    n_input_spike_trains = strtoul(argv[1], NULL, 10);
    time_steps = strtoul(argv[2], NULL, 10);
    prob = strtoul(argv[3],NULL, 10);

    // reserve memory
    n_spikes = (int *)malloc(n_input_spike_trains * sizeof(int));

    spike_trains = (int **)malloc(n_input_spike_trains * sizeof(int *));
    for(int i = 0; i<n_input_spike_trains; i++){
        spike_trains[i] = (int *)malloc(time_steps * sizeof(int));
    }

    // generate spike train
    random_input_spike_train_generator(spike_trains, n_spikes, n_input_spike_trains, time_steps, prob);

    // store generated spike trains on a file
    FILE *f;
    f = fopen("input_spikes.txt", "w");
 
    for(int i = 0; i<n_input_spike_trains; i++){
        fprintf(f, "%d ", n_spikes[i]);
        for(int j = 0; j<n_spikes[i]; j++){
            fprintf(f, "%d ", spike_trains[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}