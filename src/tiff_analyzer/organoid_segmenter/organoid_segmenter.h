#ifndef ORGANOID_SEGMENTER_H
#define ORGANOID_SEGMENTER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Performs high-precision circular organoid segmentation.
 * image_bytes: raw uint8 image pixels
 * channels: 1 (grayscale) or 3 (RGB/BGR)
 * width, height: dimensions
 * min_size: minimum pixel area for an organoid object
 * min_circularity: minimum circularity index (0.10 .. 1.00) to filter out background/irregular shapes
 * out_mask: pre-allocated uint8 array (size = width * height) to store binary mask
 * out_labels: pre-allocated int32 array (size = width * height) to store object IDs (1..N)
 * Returns: total count N of valid circular organoid objects detected
 */
int segment_organoids(
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    int min_size,
    double min_circularity,
    uint8_t* out_mask,
    int32_t* out_labels
);

#ifdef __cplusplus
}
#endif

#endif /* ORGANOID_SEGMENTER_H */
