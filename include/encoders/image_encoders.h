#ifndef IMAGE_ENCODERS_H
#define IMAGE_ENDOERS_H


/// @brief Image of spikes
typedef struct {
    int **spike_image; // first dimension is a list of pixels, and the second dimension the spike train for each pixel
} spike_image_t;

/// @brief List of spiking images
typedef struct {
    spike_image_t *spike_images; // list of images
    int image_size;
    int n_images;
    int bins;
} spike_images_t;


/// @brief 
void convert_images_to_spikes_by_poisson_distribution(spike_images_t *spike_images, double **images, int n_images, int image_size, int bins);
void convert_image_to_spikes_by_poisson_distribution(spike_image_t *spike_image, double *image, int image_size, int bins);

#endif