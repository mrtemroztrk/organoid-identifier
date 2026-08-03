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

int calculate_organoid_metrics(
    const uint8_t* mask,
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    OrganoidMetricResult* out_results,
    int max_labels
);

int calculate_multi_object_metrics(
    const int32_t* labels,
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    int num_objects,
    OrganoidMetricResult* out_results
);

#ifdef __cplusplus
}
#endif

#endif /* ORGANOID_METRICS_H */
