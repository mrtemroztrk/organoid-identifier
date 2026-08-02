#ifndef TIFF_TAGS_H
#define TIFF_TAGS_H

#include <stdint.h>

/**
 * @struct TIFFImageInfo
 * @brief Holds structural dimension metadata extracted from TIFF IFD tags.
 * 
 * @var TIFFImageInfo::width
 * Image width in pixels (Tag 256 / 0x0100).
 * @var TIFFImageInfo::height
 * Image height/length in pixels (Tag 257 / 0x0101).
 * @var TIFFImageInfo::bits_per_sample
 * Number of bits per component (Tag 258 / 0x0102).
 * @var TIFFImageInfo::is_valid
 * Status flag (1 = Tags parsed successfully, 0 = Reading error).
 */
typedef struct {
    uint32_t width;           
    uint32_t height;          
    uint16_t bits_per_sample; 
    int is_valid;             
} TIFFImageInfo;

/**
 * @brief Seeks to the IFD table and extracts image dimensions and bit depth tags.
 * 
 * @details Performs direct seek (O(1)) to the IFD offset byte location, reads the 2-byte 
 *          tag count, and iterates through 12-byte tag entries to locate width (256), 
 *          height (257), and bit depth (258).
 * 
 * @param file_path Path to the target TIFF image file.
 * @param ifd_offset Byte position where the first IFD table starts.
 * @return TIFFImageInfo Struct populated with extracted tag values.
 */
TIFFImageInfo read_tiff_tags(const char *file_path, uint32_t ifd_offset);

#endif // TIFF_TAGS_H