#include <stdio.h>
#include "tiff_tags.h"

TIFFImageInfo read_tiff_tags(const char *file_path, uint32_t ifd_offset) {
    TIFFImageInfo info;
    info.width = 0;
    info.height = 0;
    info.bits_per_sample = 0;
    info.is_valid = 0;

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return info;
    }

    if (fseek(file, ifd_offset, SEEK_SET) != 0) {
        fclose(file);
        return info;
    }

    uint16_t tag_count = 0;
    if (fread(&tag_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return info;
    }

    for (uint16_t i = 0; i < tag_count; i++) {
        uint16_t tag_id;
        uint16_t tag_type;
        uint32_t tag_count_val;
        uint32_t tag_value;

        if (fread(&tag_id, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_type, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_count_val, sizeof(uint32_t), 1, file) != 1) break;
        if (fread(&tag_value, sizeof(uint32_t), 1, file) != 1) break;

        // Tag 256: ImageWidth
        if (tag_id == 256) {
            info.width = tag_value;
        }
        // Tag 257: ImageLength (Height)
        else if (tag_id == 257) {
            info.height = tag_value;
        }
        // Tag 258: BitsPerSample DÜZELTMESİ BURADA
        else if (tag_id == 258) {
            if (tag_value <= 16) {
                info.bits_per_sample = (uint16_t)tag_value;
            } else {
                info.bits_per_sample = 8; // Kanal başı standart 8-bit varsayıyoruz
            }
        }
    }

    fclose(file);

    if (info.width > 0 && info.height > 0) {
        info.is_valid = 1;
    }

    return info;
}