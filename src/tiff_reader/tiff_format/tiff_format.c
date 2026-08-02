#include <stdio.h>
#include "tiff_format.h"

TIFFFormatInfo read_tiff_format(const char *file_path, uint32_t ifd_offset) {
    TIFFFormatInfo info = {0, 0, 0, 0};

    FILE *file = fopen(file_path, "rb");
    if (!file) return info;

    // IFD adresine doğrudan zıpla
    if (fseek(file, ifd_offset, SEEK_SET) != 0) {
        fclose(file);
        return info;
    }

    uint16_t tag_count = 0;
    if (fread(&tag_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return info;
    }

    // 12 baytlık etiketleri sırayla tara
    for (uint16_t i = 0; i < tag_count; i++) {
        uint16_t tag_id;
        uint16_t tag_type;
        uint32_t tag_count_val;
        uint32_t tag_value;

        if (fread(&tag_id, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_type, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_count_val, sizeof(uint32_t), 1, file) != 1) break;
        if (fread(&tag_value, sizeof(uint32_t), 1, file) != 1) break;

        // Tag 259: Compression
        if (tag_id == 259) {
            info.compression = (uint16_t)tag_value;
        }
        // Tag 262: PhotometricInterpretation
        else if (tag_id == 262) {
            info.photometric_interp = (uint16_t)tag_value;
        }
        // Tag 277: SamplesPerPixel (Kanal Sayısı)
        else if (tag_id == 277) {
            info.samples_per_pixel = (uint16_t)tag_value;
        }
    }

    fclose(file);
    info.is_valid = 1;
    return info;
}