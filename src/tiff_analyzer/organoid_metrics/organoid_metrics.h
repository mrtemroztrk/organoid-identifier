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
} OrganoidMetricResult;

/*
 * Calculates morphological metrics for thresholded or labeled binary mask pixels.
 * width, height: dimensions of image
 * mask: 1D array of uint8 (0 = background, >0 = organoid foreground)
 * out_results: allocated array to store results
 * max_labels: size of out_results buffer
 * Returns: number of detected organoids/regions
 */
int calculate_organoid_metrics(
    const uint8_t* mask,
    int width,
    int height,
    OrganoidMetricResult* out_results,
    int max_labels
);

#ifdef __cplusplus
}
#endif

#endif /* ORGANOID_METRICS_H */
