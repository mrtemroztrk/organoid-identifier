#include "organoid_metrics.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int calculate_organoid_metrics(
    const uint8_t* mask,
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    OrganoidMetricResult* out_results,
    int max_labels
) {
    if (!mask || width <= 0 || height <= 0 || !out_results || max_labels <= 0) {
        return 0;
    }

    if (channels <= 0) {
        channels = 1;
    }

    size_t area = 0;
    size_t perimeter = 0;
    double sum_x = 0.0;
    double sum_y = 0.0;
    int min_x = width, min_y = height;
    int max_x = -1, max_y = -1;

    double integrated_intensity = 0.0;
    double min_intensity = 1e9;
    double max_intensity = -1e9;

    // Pass 1: Area, perimeter, centroid, bounding box, integrated & min/max intensity
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            if (mask[idx] > 0) {
                area++;
                sum_x += x;
                sum_y += y;

                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;

                // Pixel-wise signal intensity calculation
                double intensity = 0.0;
                if (image_bytes) {
                    if (channels == 1) {
                        intensity = (double)image_bytes[idx];
                    } else if (channels >= 3) {
                        int p = idx * channels;
                        // Standard BT.601 luminance weighting: 0.299*R + 0.587*G + 0.114*B
                        intensity = 0.299 * (double)image_bytes[p] +
                                    0.587 * (double)image_bytes[p + 1] +
                                    0.114 * (double)image_bytes[p + 2];
                    }
                } else {
                    intensity = (double)mask[idx];
                }

                integrated_intensity += intensity;
                if (intensity < min_intensity) min_intensity = intensity;
                if (intensity > max_intensity) max_intensity = intensity;

                // Check 4-neighbor connectivity for boundary/perimeter
                int is_boundary = 0;
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    is_boundary = 1;
                } else {
                    if (mask[idx - 1] == 0 || mask[idx + 1] == 0 ||
                        mask[idx - width] == 0 || mask[idx + width] == 0) {
                        is_boundary = 1;
                    }
                }
                if (is_boundary) {
                    perimeter++;
                }
            }
        }
    }

    if (area == 0) {
        return 0;
    }

    double mean_intensity = integrated_intensity / (double)area;

    // Pass 2: Calculate standard deviation of intensity (heterogeneity)
    double sum_sq_diff = 0.0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            if (mask[idx] > 0) {
                double intensity = 0.0;
                if (image_bytes) {
                    if (channels == 1) {
                        intensity = (double)image_bytes[idx];
                    } else if (channels >= 3) {
                        int p = idx * channels;
                        intensity = 0.299 * (double)image_bytes[p] +
                                    0.587 * (double)image_bytes[p + 1] +
                                    0.114 * (double)image_bytes[p + 2];
                    }
                } else {
                    intensity = (double)mask[idx];
                }
                double diff = intensity - mean_intensity;
                sum_sq_diff += diff * diff;
            }
        }
    }
    double std_intensity = sqrt(sum_sq_diff / (double)area);

    out_results[0].label_id = 1;
    out_results[0].area = area;
    out_results[0].perimeter = perimeter;
    out_results[0].centroid_x = sum_x / (double)area;
    out_results[0].centroid_y = sum_y / (double)area;
    out_results[0].min_x = min_x;
    out_results[0].min_y = min_y;
    out_results[0].max_x = max_x;
    out_results[0].max_y = max_y;

    out_results[0].mean_intensity = mean_intensity;
    out_results[0].integrated_intensity = integrated_intensity;
    out_results[0].min_intensity = (min_intensity > 1e8) ? 0.0 : min_intensity;
    out_results[0].max_intensity = (max_intensity < -1e8) ? 0.0 : max_intensity;
    out_results[0].std_intensity = std_intensity;

    // Circularity: (4 * PI * Area) / (Perimeter^2)
    if (perimeter > 0) {
        out_results[0].circularity = (4.0 * M_PI * (double)area) / ((double)perimeter * (double)perimeter);
        if (out_results[0].circularity > 1.0) {
            out_results[0].circularity = 1.0;
        }
    } else {
        out_results[0].circularity = 0.0;
    }

    // Equivalent Diameter: 2 * sqrt(Area / PI)
    out_results[0].equiv_diameter = 2.0 * sqrt((double)area / M_PI);

    return 1;
}
