#include "organoid_segmenter.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    double min_circularity,
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

    // Border mean intensity to detect polarity (bright vs dark background)
    double border_sum = 0.0;
    int border_count = 0;
    for (int x = 0; x < width; x++) {
        border_sum += gray[x] + gray[(height - 1) * width + x];
        border_count += 2;
    }
    for (int y = 0; y < height; y++) {
        border_sum += gray[y * width] + gray[y * width + (width - 1)];
        border_count += 2;
    }
    double border_mean = border_sum / (double)border_count;
    int bright_background = (border_mean > 128.0) ? 1 : 0;

    int threshold = compute_otsu_threshold(gray, total_pixels);

    // Temp label buffer
    int32_t* temp_labels = (int32_t*)calloc(total_pixels, sizeof(int32_t));
    int* queue_x = (int*)malloc(total_pixels * sizeof(int));
    int* queue_y = (int*)malloc(total_pixels * sizeof(int));

    if (!temp_labels || !queue_x || !queue_y) {
        free(gray);
        if (temp_labels) free(temp_labels);
        if (queue_x) free(queue_x);
        if (queue_y) free(queue_y);
        return 0;
    }

    int raw_label = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            int is_fg = bright_background ? (gray[idx] <= threshold) : (gray[idx] >= threshold);

            if (is_fg && temp_labels[idx] == 0) {
                raw_label++;
                int head = 0, tail = 0;

                queue_x[tail] = x;
                queue_y[tail] = y;
                tail++;
                temp_labels[idx] = raw_label;

                int touches_border = 0;
                size_t component_size = 0;

                while (head < tail) {
                    int cx = queue_x[head];
                    int cy = queue_y[head];
                    head++;
                    component_size++;

                    if (cx == 0 || cx == width - 1 || cy == 0 || cy == height - 1) {
                        touches_border = 1;
                    }

                    int dx[] = {-1, 1, 0, 0};
                    int dy[] = {0, 0, -1, 1};

                    for (int d = 0; d < 4; d++) {
                        int nx = cx + dx[d];
                        int ny = cy + dy[d];

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            int nidx = ny * width + nx;
                            int n_fg = bright_background ? (gray[nidx] <= threshold) : (gray[nidx] >= threshold);
                            if (n_fg && temp_labels[nidx] == 0) {
                                temp_labels[nidx] = raw_label;
                                queue_x[tail] = nx;
                                queue_y[tail] = ny;
                                tail++;
                            }
                        }
                    }
                }

                // Check size and border touching
                if (touches_border || component_size < (size_t)min_size || component_size > (size_t)(total_pixels * 0.35)) {
                    for (int k = 0; k < tail; k++) {
                        int cidx = queue_y[k] * width + queue_x[k];
                        temp_labels[cidx] = -1; // invalid
                    }
                    continue;
                }

                // Calculate perimeter to evaluate circularity
                size_t perimeter = 0;
                for (int k = 0; k < tail; k++) {
                    int px = queue_x[k];
                    int py = queue_y[k];
                    int pidx = py * width + px;

                    int is_b = 0;
                    if (px == 0 || px == width - 1 || py == 0 || py == height - 1) {
                        is_b = 1;
                    } else {
                        if (temp_labels[pidx - 1] != raw_label || temp_labels[pidx + 1] != raw_label ||
                            temp_labels[pidx - width] != raw_label || temp_labels[pidx + width] != raw_label) {
                            is_b = 1;
                        }
                    }
                    if (is_b) perimeter++;
                }

                double circularity = 0.0;
                if (perimeter > 0) {
                    circularity = (4.0 * M_PI * (double)component_size) / ((double)perimeter * (double)perimeter);
                    if (circularity > 1.0) circularity = 1.0;
                }

                if (circularity < min_circularity) {
                    for (int k = 0; k < tail; k++) {
                        int cidx = queue_y[k] * width + queue_x[k];
                        temp_labels[cidx] = -1; // invalid shape
                    }
                }
            }
        }
    }

    // Final relabeling of valid objects
    int valid_count = 0;
    int* label_map = (int*)calloc(raw_label + 1, sizeof(int));

    for (int i = 0; i < total_pixels; i++) {
        int l = temp_labels[i];
        if (l > 0) {
            if (label_map[l] == 0) {
                valid_count++;
                label_map[l] = valid_count;
            }
            out_labels[i] = label_map[l];
            out_mask[i] = 255;
        }
    }

    free(gray);
    free(temp_labels);
    free(queue_x);
    free(queue_y);
    free(label_map);

    return valid_count;
}
