#include "organoid_segmenter.h"
#include <stdlib.h>
#include <string.h>

static int compute_otsu_threshold(const uint8_t* gray, int total_pixels) {
    int histogram[256] = {0};
    for (int i = 0; i < total_pixels; i++) {
        histogram[gray[i]]++;
    }

    double sum = 0;
    for (int t = 0; t < 256; t++) {
        sum += t * histogram[t];
    }

    double sumB = 0;
    int wB = 0;
    int wF = 0;
    double varMax = 0;
    int threshold = 128;

    for (int t = 0; t < 256; t++) {
        wB += histogram[t];
        if (wB == 0) continue;
        wF = total_pixels - wB;
        if (wF == 0) break;

        sumB += (double)(t * histogram[t]);
        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }
    return threshold;
}

int segment_organoids(
    const uint8_t* image_bytes,
    int channels,
    int width,
    int height,
    int min_size,
    uint8_t* out_mask,
    int32_t* out_labels
) {
    if (!image_bytes || width <= 0 || height <= 0 || !out_mask || !out_labels) {
        return 0;
    }

    int total_pixels = width * height;
    memset(out_mask, 0, total_pixels * sizeof(uint8_t));
    memset(out_labels, 0, total_pixels * sizeof(int32_t));

    uint8_t* gray = (uint8_t*)malloc(total_pixels);
    if (!gray) return 0;

    for (int i = 0; i < total_pixels; i++) {
        if (channels == 1) {
            gray[i] = image_bytes[i];
        } else if (channels >= 3) {
            int p = i * channels;
            gray[i] = (uint8_t)(0.299 * image_bytes[p] + 0.587 * image_bytes[p + 1] + 0.114 * image_bytes[p + 2]);
        }
    }

    int threshold = compute_otsu_threshold(gray, total_pixels);
    // Ensure threshold isn't zero
    if (threshold < 10) threshold = 10;

    // Queue for BFS flood fill
    int* queue_x = (int*)malloc(total_pixels * sizeof(int));
    int* queue_y = (int*)malloc(total_pixels * sizeof(int));

    if (!queue_x || !queue_y) {
        free(gray);
        if (queue_x) free(queue_x);
        if (queue_y) free(queue_y);
        return 0;
    }

    int current_label = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            if (gray[idx] >= threshold && out_labels[idx] == 0) {
                current_label++;
                int head = 0, tail = 0;
                
                queue_x[tail] = x;
                queue_y[tail] = y;
                tail++;
                out_labels[idx] = current_label;

                int component_size = 0;

                while (head < tail) {
                    int cx = queue_x[head];
                    int cy = queue_y[head];
                    head++;
                    component_size++;

                    // 4-neighborhood
                    int dx[] = {-1, 1, 0, 0};
                    int dy[] = {0, 0, -1, 1};

                    for (int d = 0; d < 4; d++) {
                        int nx = cx + dx[d];
                        int ny = cy + dy[d];

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            int nidx = ny * width + nx;
                            if (gray[nidx] >= threshold && out_labels[nidx] == 0) {
                                out_labels[nidx] = current_label;
                                queue_x[tail] = nx;
                                queue_y[tail] = ny;
                                tail++;
                            }
                        }
                    }
                }

                // If component size is smaller than min_size, clear label
                if (component_size < min_size) {
                    for (int k = 0; k < tail; k++) {
                        int cidx = queue_y[k] * width + queue_x[k];
                        out_labels[cidx] = 0;
                    }
                    current_label--;
                }
            }
        }
    }

    // Populate binary mask for remaining valid objects
    for (int i = 0; i < total_pixels; i++) {
        if (out_labels[i] > 0) {
            out_mask[i] = 255;
        }
    }

    free(gray);
    free(queue_x);
    free(queue_y);

    return current_label;
}
