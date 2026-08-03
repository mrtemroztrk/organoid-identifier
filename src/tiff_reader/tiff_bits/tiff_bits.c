#include <stdio.h>
#include "tiff_bits.h"

TIFFBitsInfo read_tiff_bits(const char *file_path, uint32_t ifd_offset) {
    TIFFBitsInfo info = {8, 1, 0};

    FILE *file = fopen(file_path, "rb");
    if (!file) return info;

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
        uint16_t tag_id, tag_type;
        uint32_t tag_count_val, tag_val;

        if (fread(&tag_id, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_type, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_count_val, sizeof(uint32_t), 1, file) != 1) break;
        if (fread(&tag_val, sizeof(uint32_t), 1, file) != 1) break;

        // Tag 258: BitsPerSample
        if (tag_id == 258) {
            // Eğer tek kanallı veya değer doğrudan tag alanına sığdıysa
            if (tag_count_val == 1) {
                info.bits_per_sample = (uint16_t)(tag_val & 0xFFFF);
            } else {
                // Birden fazla kanal varsa tag_val bir offset adresidir, adrese gidip ilk kanalı oku
                long current_pos = ftell(file);
                if (fseek(file, tag_val, SEEK_SET) == 0) {
                    uint16_t bit_depth = 8;
                    if (fread(&bit_depth, sizeof(uint16_t), 1, file) == 1) {
                        info.bits_per_sample = bit_depth;
                    }
                    fseek(file, current_pos, SEEK_SET); // Kaldığın yere geri dön
                }
            }
        }
        // Tag 339: SampleFormat
        else if (tag_id == 339) {
            info.sample_format = (uint16_t)(tag_val & 0xFFFF);
        }
    }

    fclose(file);
    info.is_valid = 1;
    return info;
}