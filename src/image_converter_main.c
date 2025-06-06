#include "snn_library.h"
#include "load_data.h"
#include "helpers.h"

//#include "neuron_models/GPU_lif_neuron.cuh"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../MNIST_for_C/mnist.h"
#include "encoders/image_encoders.h"


/* main.c */

int main(int argc, char *argv[]) {
    // randomize execution
    srand(time(NULL));

    int i, j;

    load_mnist();

    int *train_labels, *test_labels;
    int n_train_images = 60000, n_test_images = 10000;

    train_labels = (int *)malloc(n_train_images * sizeof(int));
    test_labels = (int *)malloc(n_test_images * sizeof(int));


    // read labels from the file
    for(i=0; i<n_train_images; i++){
        train_labels[i] = train_label[i];
    }    
    
    // read labels from the file
    for(i=0; i<n_test_images; i++){
        test_labels[i] = test_label[i];
    }    

    // store generated spike trains
    FILE *train_set_output_file, *test_set_output_file;
    
    train_set_output_file = fopen("./data/MNIST/train/labels.txt", "w");
    if(train_set_output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");

    for(i = 0; i<n_train_images; i++){
        fprintf(train_set_output_file, "%d ", train_labels[i]);
    }

    // test
    test_set_output_file = fopen("./data/MNIST/test/labels.txt", "w");
    if(test_set_output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");

    for(i = 0; i<n_test_images; i++){
        fprintf(test_set_output_file, "%d ", test_labels[i]);
    }


    return 0;
}

/*int main(int argc, char *argv[]) {    
    // randomize execution
    srand(time(NULL));

    int i,j;

    // load dataset to convert into spikes
    load_mnist(); // load information from mnist dataset

    spike_images_t train_spike_images, test_spike_images;

    // compute bins
    int pT = 100;
    int pDT = 1;
    int bins = (int)(pT/pDT);

    // set parameters for datasets
    train_spike_images.bins = bins;
    train_spike_images.image_size = 784;
    train_spike_images.n_images = 60000;

    test_spike_images.bins = bins;
    test_spike_images.image_size = 784;
    test_spike_images.n_images = 10000;

    
    train_spike_images.spike_images = malloc(train_spike_images.n_images * sizeof(spike_image_t)); // alloc memory for train images
    test_spike_images.spike_images = malloc(test_spike_images.n_images * sizeof(spike_image_t)); // alloc memory for test images


    // alloc memory for datasets
    printf("Reserving memory for datasets...\n");
    for(i=0; i<train_spike_images.n_images; i++){
        // alloc memory for image pixels
        train_spike_images.spike_images[i].spike_image = malloc(train_spike_images.image_size * sizeof(int *)); // each image has 728 pixels (this must be generalized)

        for(j=0; j<train_spike_images.image_size; j++){ // alloc bins integers for each pixel spike train --> this must be corrected, as bins is used for 0,1 spike trains, but I use spike times, so less positions are needed
            train_spike_images.spike_images[i].spike_image[j] = malloc(bins * sizeof(int));
        }
    }
    for(i=0; i<test_spike_images.n_images; i++){
        // alloc memory for image pixels
        test_spike_images.spike_images[i].spike_image = malloc(test_spike_images.image_size * sizeof(int *)); // each image has 728 pixels (this must be generalized)

        for(j=0; j<test_spike_images.image_size; j++){ // alloc bins integers for each pixel spike train --> this must be corrected, as bins is used for 0,1 spike trains, but I use spike times, so less positions are needed
            test_spike_images.spike_images[i].spike_image[j] = malloc(bins * sizeof(int));
        }
    }
    printf("Memory reserved!\n");

    // convert images into spike images
    printf("Converting train partition images to spike trains...\n");
    convert_images_to_spikes_by_poisson_distribution(&train_spike_images, train_image, train_spike_images.n_images, train_spike_images.image_size, train_spike_images.bins);
    printf("Conversion correctly completed!\n");


    // convert images into spike images
    printf("Converting test partition images to spike trains...\n");
    convert_images_to_spikes_by_poisson_distribution(&test_spike_images, test_image, test_spike_images.n_images, test_spike_images.image_size, test_spike_images.bins);
    printf("Conversion correctly completed!\n");


    // store generated spike trains
    FILE *train_set_output_file, *test_set_output_file;
    
    train_set_output_file = fopen(argv[1], "w");
    if(train_set_output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");

    for(i = 0; i<train_spike_images.n_images; i++){
        for(j = 0; j<train_spike_images.image_size; j++){
            for(int l=0; l<train_spike_images.spike_images[i].spike_image[j][0]+1; l++){
                fprintf(train_set_output_file, "%d ", train_spike_images.spike_images[i].spike_image[j][l]);
            }
            fprintf(train_set_output_file, "\n");
        }
        fprintf(train_set_output_file, "\n");
    }

    // test
    test_set_output_file = fopen(argv[2], "w");
    if(test_set_output_file == NULL){
        printf("Error opening the file. \n");
        exit(1);
    }
    printf("File openned\n");

    for(i = 0; i<test_spike_images.n_images; i++){
        for(j = 0; j<test_spike_images.image_size; j++){
            for(int l=0; l<test_spike_images.spike_images[i].spike_image[j][0]+1; l++){
                fprintf(test_set_output_file, "%d ", test_spike_images.spike_images[i].spike_image[j][l]);
            }
            fprintf(test_set_output_file, "\n");
        }
        fprintf(test_set_output_file, "\n");
    }


    return 0;
}*/
