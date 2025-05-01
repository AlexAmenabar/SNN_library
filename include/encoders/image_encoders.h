#ifndef IMAGE_ENCODERS_H
#define IMAGE_ENDOERS_H


/** 
This file contains the structures and functions to convert different types of input into spike trains and store them.
*/


/// @brief Image of spikes. Each image has a list of spikes for each pixel
typedef struct {
    int **image; // first dimension is a list of pixels, and the second dimension the spike train for each pixel
} spike_image_t;

/// @brief List of images converted into spikes
typedef struct {
    spike_image_t *images; // list of images
    int image_size;
    int n_images;
    int bins;
} image_dataset_t;


/// @brief Convert dataset of images into spike images
/// @param spike_images Struct to store the image converted
/// @param images Images to be converted into spikes
/// @param n_images Number of images in the dataset to be converted into spikes
/// @param image_size Number of pixels into the image
/// @param bins number of spikes (max) into each pixel spike train
void convert_images_to_spikes_by_poisson_distribution(image_dataset_t *spike_images, double **images, int n_images, int image_size, int bins);


/// @brief Convert a image into spikes
/// @param spike_image Struct to store the image converted
/// @param image Image to be converted into spikes
/// @param image_size Number of pixels into the image
/// @param bins number of spikes (max) into each pixel spike train
void convert_image_to_spikes_by_poisson_distribution(spike_image_t *spike_image, double *image, int image_size, int bins);

#endif