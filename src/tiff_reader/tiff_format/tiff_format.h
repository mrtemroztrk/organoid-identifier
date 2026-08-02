#ifndef TIFF_FORMAT_H
#define TIFF_FORMAT_H

#include <stdint.h>

/**
 * @struct TIFFFormatInfo
 * @brief Holds image color space, channel count, and compression metadata.
 */
typedef struct {
    uint16_t samples_per_pixel;       // Tag 277: Kanal Sayısı (1=Single, 3=RGB)
    uint16_t photometric_interp;     // Tag 262: Renk Modeli (0/1=Grayscale, 2=RGB)
    uint16_t compression;            // Tag 259: Sıkıştırma Tipi (1=None, 5=LZW)
    int is_valid;                    // Status flag
} TIFFFormatInfo;

/**
 * @brief Reads IFD tags to extract image color space and format details.
 */
TIFFFormatInfo read_tiff_format(const char *file_path, uint32_t ifd_offset);

#endif // TIFF_FORMAT_H