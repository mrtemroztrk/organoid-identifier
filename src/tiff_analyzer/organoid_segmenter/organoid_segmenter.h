#ifndef ORGANOID_SEGMENTER_H
#define ORGANOID_SEGMENTER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Performs zero-dependency connected-component segmentation on image pixels.
 * image_bytes: raw uint8 image pixels
 * channels: 1 (grayscale) or 3 (RGB/BGR)
 * width, height: dimensions
 * min_size: minimum pixel area threshold for an object to be counted
 * out_mask: pre-allocated uint8 array (size = width * height) to store binary mask (0 or 255)
 * out_labels: pre-allocated int32 array (size = width * height) to store object IDs (0 = bg, 1..N = object ID)
 * Returns: total count of segmented organoid objects N
 */
int segment_organoids(
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    int min_size,
    uint8_t* out_mask,
    int32_t* out_labels
);

#ifdef __cplusplus
}
#endif

#endif /* ORGANOID_SEGMENTER_H */
