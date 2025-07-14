#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define N_TRAIN 1000
#define N_TEST 1797

// 3823

int main(int argc, char *argv[]) {

    int pixel_value;
    int train_images[N_TRAIN][64];
    //int test_images[N_TRAIN][64];
    int train_spike_images[N_TRAIN][64][18];
    //int test_spike_images[N_TEST][63][32];

    int train_labels[N_TRAIN];
    //int test_labels[N_TEST];

    int bins = 16;

    // Open the file that constains 
    FILE *f, *df, *lf;
    f = fopen("./Original/train.txt", "r");
    if (f == NULL){
        perror("Error opening the file 1\n");
        exit(1);
        return 1;
    }
    
    df = fopen("./Spikes/train_images.txt", "w");
    if (df == NULL){
        perror("Error opening the file 2\n");
        exit(1);
        return 1;
    }

    lf = fopen("./Spikes/train_labels.txt", "w");
    if (lf == NULL){
        perror("Error opening the file 3\n");
        exit(1);
        return 1;
    }

    printf("Transforming...\n");

    int next_index = 0;
    int n_spikes = 0;
    // read data, convert to spikes, and store
    for(int i = 0; i<N_TRAIN; i++){
        for(int j = 0; j<64; j++){

            next_index = 0;
            
            // read pixel value
            fscanf(f, "%d", &(pixel_value));

            for(int s = 0; s<16; s++){
                double random_number = (double)rand() / (double)RAND_MAX * 16;
                //printf("random_number %f\n", random_number);
                if(pixel_value > random_number)
                {
                    next_index ++;
                    train_spike_images[i][j][next_index] = s; // add spike on time t 
                }
            }
            train_spike_images[i][j][0] = next_index;
        }
        
        fscanf(f, "%d", &(train_labels[i])); 
    }

    // close file
    fclose(f);

    printf("Transformed!\n");


    printf("Storing\n");
    // store results in file
    for(int i = 0; i<N_TRAIN; i++){
        for(int j = 0; j<64; j++){
            fprintf(df, "%d ", train_spike_images[i][j][0]);

            for(int s = 0; s<train_spike_images[i][j][0]; s++){
                fprintf(df, "%d ", train_spike_images[i][j][s+1]);
            }
            
            fprintf(df, "\n");
        }

        fprintf(lf, "%d ", train_labels[i]);
    }

    printf("Stored!\n");
}