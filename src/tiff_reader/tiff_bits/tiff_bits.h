#ifndef TIFF_BITS_H
#define TIFF_BITS_H

#include <stdint.h>

typedef struct {
    uint16_t bits_per_sample;
    uint16_t sample_format;
    int is_valid;
} TIFFBitsInfo;

TIFFBitsInfo read_tiff_bits(const char *file_path, uint32_t ifd_offset);

#endif // TIFF_BITS_H