#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define N_TRAIN 3823;
#define N_TEST 1797;


void read_data(){

}

void write_data(){

}

int main(int argc, char *argv[]) {

    int train_images[N_TRAIN][64];
    int test_images[N_TRAIN][64];

    int train_labels[N_TRAIN];
    int test_labels[N_TEST];

    // Open the file that constains 
    FILE *f;
    *f = fopen("./HandwritenDigits/Original/optdigits.tra", "r");
    if (*f == NULL){
        perror("Error opening the file\n");
        exit(1);
        return 1;
    }
    
    


    // close file
    fclose(f);


    FILE *f;
    *f = fopen("./HandwritenDigits/Original/optdigits.tra", "r");
    if (*f == NULL){
        perror("Error opening the file\n");
        exit(1);
        return 1;
    }
    
    fclose(f);

}