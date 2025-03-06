#include "encoders/image_encoders.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/// Images are arrays of arrays, first dimension for different images, and the
/// second one for image pixels
void convert_images_to_spikes_by_poisson_distribution(spike_images_t *spike_images, double **images, int n_images, int image_size, int bins){
    int i;

    // convert all images into spikes
    for(i=0; i<n_images; i++){
        convert_image_to_spikes_by_poisson_distribution(&(spike_images->spike_images[i]), images[i], image_size, bins);
    }
}

void convert_image_to_spikes_by_poisson_distribution(spike_image_t *spike_image, double *image, int image_size, int bins){
    // convert each pixel of the image into spike_train
    double random_number;
    int next_index;
    for(int i = 0; i<image_size; i++){
        next_index = 0;
        // image[j] is a value, a pixel value
        // spikes[...] is the spike train
        for(int j = 0; j<bins; j++){
            random_number = (double)rand() / (double)RAND_MAX;
            if(image[i] > random_number)
            {
                next_index ++;
                spike_image->spike_image[i][next_index] = j; // add spike on time t
                
            }
            //else
            //    spike_image->spike_image[i][next_index] = 0;
            
        }
        spike_image->spike_image[i][0] = next_index; // store amount of spikes in spike train
    }
}