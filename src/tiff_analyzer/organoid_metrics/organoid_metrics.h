#ifndef ORGANOID_METRICS_H
#define ORGANOID_METRICS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int label_id;
    size_t area;
    size_t perimeter;
    double circularity;
    double equiv_diameter;
    double centroid_x;
    double centroid_y;
    int min_x;
    int min_y;
    int max_x;
    int max_y;
    double mean_intensity;
    double integrated_intensity;
    double min_intensity;
    double max_intensity;
    double std_intensity;
} OrganoidMetricResult;

/*
 * Calculates morphological & pixel-wise signal intensity metrics for organoids.
 * mask: 1D array of uint8 (0 = background, >0 = organoid foreground)
 * image_bytes: optional 1D array of uint8 image pixels (can be NULL)
 * channels: 1 (grayscale) or 3 (RGB/BGR), used if image_bytes is provided
 * width, height: image dimensions
 * out_results: allocated array to store results
 * max_labels: size of out_results buffer
 * Returns: number of detected organoids/regions
 */
int calculate_organoid_metrics(
    const uint8_t* mask,
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    OrganoidMetricResult* out_results,
    int max_labels
);

#ifdef __cplusplus
}
#endif

#endif /* ORGANOID_METRICS_H */
