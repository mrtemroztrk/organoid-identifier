#ifndef TIFF_HEADER_H
#define TIFF_HEADER_H

#include <stdint.h>

/**
 * @struct TIFFHeader
 * @brief Holds binary metadata parsed from the initial 8-byte TIFF file header.
 * 
 * @var TIFFHeader::byte_order
 * 2-byte marker. 0x4949 ('II') for Little-Endian, 0x4D4D ('MM') for Big-Endian.
 * @var TIFFHeader::magic_number
 * 2-byte validation code. Must equal 42 (0x002A) for standard TIFF specification.
 * @var TIFFHeader::ifd_offset
 * 4-byte unsigned integer indicating the absolute byte position of the 1st IFD.
 * @var TIFFHeader::is_valid
 * Boolean flag (1 = Valid TIFF, 0 = Invalid structure or read error).
 */
typedef struct {
    uint16_t byte_order;   
    uint16_t magic_number; 
    uint32_t ifd_offset;   
    int is_valid;          
} TIFFHeader;

/**
 * @brief Opens a binary file and reads its 8-byte TIFF header into memory.
 * 
 * @details Performs non-destructive O(1) binary read. Opens the file in binary mode ("rb")
 *          and extracts byte order, magic number, and IFD offset sequentially without
 *          loading image payload into memory.
 * 
 * @param file_path Path to the target TIFF file.
 * @return TIFFHeader Struct populated with extracted metadata and validity status.
 */
TIFFHeader read_tiff_header(const char *file_path);

#endif // TIFF_HEADER_H