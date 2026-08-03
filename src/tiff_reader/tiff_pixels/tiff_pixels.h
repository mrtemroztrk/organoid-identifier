#ifndef TIFF_PIXELS_H
#define TIFF_PIXELS_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint16_t channels;
    uint8_t *pixel_bytes;
    size_t data_size;
    int is_valid;
} TIFFPixelData;

TIFFPixelData read_tiff_pixels(const char *file_path, uint32_t ifd_offset);
void free_tiff_pixels(TIFFPixelData *data);

#endif // TIFF_PIXELS_H