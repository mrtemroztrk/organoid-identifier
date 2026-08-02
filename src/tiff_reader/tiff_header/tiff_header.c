#include <stdio.h>
#include "tiff_header.h"

TIFFHeader read_tiff_header(const char *file_path) {
    TIFFHeader header;
    header.is_valid = 0; // Default: Invalid state

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return header; // File read error
    }

    // Byte 0-1: Byte Order (uint16)
    if (fread(&header.byte_order, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return header;
    }

    // Byte 2-3: Magic Number (uint16)
    if (fread(&header.magic_number, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return header;
    }

    // Byte 4-7: First IFD Offset (uint32)
    if (fread(&header.ifd_offset, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return header;
    }

    fclose(file);

    // TIFF Specification Validation Check
    if (header.magic_number == 42 && 
       (header.byte_order == 0x4949 || header.byte_order == 0x4D4D)) {
        header.is_valid = 1;
    }

    return header;
}