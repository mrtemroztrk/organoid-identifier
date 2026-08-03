#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiff_pixels.h"

typedef struct {
    uint16_t prefix;
    uint8_t suffix;
    uint8_t first_char;
} LZWEntry;

static size_t decode_lzw_string(const LZWEntry *table, uint32_t code, uint8_t *buf) {
    size_t len = 0;
    uint32_t c = code;
    while (c < 4096 && c != 4096) {
        buf[len++] = table[c].suffix;
        if (c < 256) break;
        c = table[c].prefix;
    }
    for (size_t i = 0; i < len / 2; i++) {
        uint8_t tmp = buf[i];
        buf[i] = buf[len - 1 - i];
        buf[len - 1 - i] = tmp;
    }
    return len;
}

static size_t decompress_lzw_strip(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_capacity) {
    if (!in || !out || in_size == 0 || out_capacity == 0) return 0;

    LZWEntry table[4096];
    for (int i = 0; i < 256; i++) {
        table[i].prefix = 4096;
        table[i].suffix = (uint8_t)i;
        table[i].first_char = (uint8_t)i;
    }

    size_t bit_pos = 0;
    size_t in_bits = in_size * 8;
    size_t out_pos = 0;

    int code_size = 9;
    uint32_t next_code = 258;
    int old_code = -1;
    uint8_t stack[4096];

    while (bit_pos + code_size <= in_bits && out_pos < out_capacity) {
        size_t byte_pos = bit_pos / 8;
        size_t bit_offset = bit_pos % 8;

        uint32_t tmp = 0;
        for (int i = 0; i < 4; i++) {
            tmp = (tmp << 8) | ((byte_pos + i < in_size) ? in[byte_pos + i] : 0);
        }
        uint32_t code = (tmp >> (32 - bit_offset - code_size)) & ((1U << code_size) - 1);
        bit_pos += code_size;

        if (code == 257) break; // EOI
        if (code == 256) { // Clear
            code_size = 9;
            next_code = 258;
            old_code = -1;
            continue;
        }

        if (old_code == -1) {
            if (code < 256) {
                out[out_pos++] = (uint8_t)code;
                old_code = code;
            }
            continue;
        }

        uint32_t in_code = code;
        if (code < next_code) {
            size_t len = decode_lzw_string(table, code, stack);
            for (size_t i = 0; i < len && out_pos < out_capacity; i++) {
                out[out_pos++] = stack[i];
            }
            if (next_code < 4096) {
                table[next_code].prefix = (uint16_t)old_code;
                table[next_code].suffix = stack[0];
                table[next_code].first_char = table[old_code].first_char;
                next_code++;
                if (next_code == 511 && code_size < 12) code_size = 10;
                else if (next_code == 1023 && code_size < 12) code_size = 11;
                else if (next_code == 2047 && code_size < 12) code_size = 12;
            }
            old_code = in_code;
        } else if (code == next_code && old_code != -1) {
            size_t len = decode_lzw_string(table, old_code, stack);
            uint8_t first_char = stack[0];
            stack[len++] = first_char;
            for (size_t i = 0; i < len && out_pos < out_capacity; i++) {
                out[out_pos++] = stack[i];
            }
            if (next_code < 4096) {
                table[next_code].prefix = (uint16_t)old_code;
                table[next_code].suffix = first_char;
                table[next_code].first_char = table[old_code].first_char;
                next_code++;
                if (next_code == 511 && code_size < 12) code_size = 10;
                else if (next_code == 1023 && code_size < 12) code_size = 11;
                else if (next_code == 2047 && code_size < 12) code_size = 12;
            }
            old_code = in_code;
        } else {
            break;
        }
    }
    return out_pos;
}

TIFFPixelData read_tiff_pixels(const char *file_path, uint32_t ifd_offset) {
    TIFFPixelData data = {0, 0, 1, NULL, 0, 0};

    FILE *file = fopen(file_path, "rb");
    if (!file) return data;

    if (fseek(file, ifd_offset, SEEK_SET) != 0) {
        fclose(file);
        return data;
    }

    uint16_t tag_count = 0;
    if (fread(&tag_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return data;
    }

    uint32_t strip_offsets_val = 0, strip_offsets_count = 0;
    uint16_t strip_offsets_type = 0;
    uint32_t strip_bytes_val = 0, strip_bytes_count = 0;
    uint16_t strip_bytes_type = 0;

    uint16_t compression = 1;
    uint16_t predictor = 1;
    uint32_t rows_per_strip = 0;

    for (uint16_t i = 0; i < tag_count; i++) {
        uint16_t tag_id, tag_type;
        uint32_t tag_count_val, tag_val;

        if (fread(&tag_id, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_type, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_count_val, sizeof(uint32_t), 1, file) != 1) break;
        if (fread(&tag_val, sizeof(uint32_t), 1, file) != 1) break;

        if (tag_id == 256) data.width = tag_val;
        else if (tag_id == 257) data.height = tag_val;
        else if (tag_id == 259) compression = (uint16_t)(tag_val & 0xFFFF);
        else if (tag_id == 277) data.channels = (uint16_t)(tag_val & 0xFFFF);
        else if (tag_id == 278) rows_per_strip = tag_val;
        else if (tag_id == 317) predictor = (uint16_t)(tag_val & 0xFFFF);
        else if (tag_id == 273) {
            strip_offsets_val = tag_val;
            strip_offsets_count = tag_count_val;
            strip_offsets_type = tag_type;
        }
        else if (tag_id == 279) {
            strip_bytes_val = tag_val;
            strip_bytes_count = tag_count_val;
            strip_bytes_type = tag_type;
        }
    }

    if (data.width == 0 || data.height == 0 || strip_offsets_count == 0 || strip_bytes_count == 0) {
        fclose(file);
        return data;
    }

    if (data.channels == 0) data.channels = 1;
    if (rows_per_strip == 0) rows_per_strip = data.height;

    uint32_t num_strips = strip_offsets_count;
    uint32_t *offsets = (uint32_t *)malloc(num_strips * sizeof(uint32_t));
    uint32_t *counts = (uint32_t *)malloc(num_strips * sizeof(uint32_t));

    if (!offsets || !counts) {
        free(offsets); free(counts);
        fclose(file);
        return data;
    }

    if (strip_offsets_count == 1) {
        offsets[0] = strip_offsets_val;
    } else {
        fseek(file, strip_offsets_val, SEEK_SET);
        for (uint32_t i = 0; i < num_strips; i++) {
            if (strip_offsets_type == 3) {
                uint16_t v; if (fread(&v, 2, 1, file) == 1) offsets[i] = v; else offsets[i] = 0;
            } else {
                uint32_t v; if (fread(&v, 4, 1, file) == 1) offsets[i] = v; else offsets[i] = 0;
            }
        }
    }

    if (strip_bytes_count == 1) {
        counts[0] = strip_bytes_val;
    } else {
        fseek(file, strip_bytes_val, SEEK_SET);
        for (uint32_t i = 0; i < num_strips; i++) {
            if (strip_bytes_type == 3) {
                uint16_t v; if (fread(&v, 2, 1, file) == 1) counts[i] = v; else counts[i] = 0;
            } else {
                uint32_t v; if (fread(&v, 4, 1, file) == 1) counts[i] = v; else counts[i] = 0;
            }
        }
    }

    size_t total_pixel_bytes = (size_t)data.width * data.height * data.channels;
    data.pixel_bytes = (uint8_t *)malloc(total_pixel_bytes);
    if (!data.pixel_bytes) {
        free(offsets); free(counts);
        fclose(file);
        return data;
    }
    memset(data.pixel_bytes, 0, total_pixel_bytes);

    size_t current_out_offset = 0;
    for (uint32_t i = 0; i < num_strips && current_out_offset < total_pixel_bytes; i++) {
        uint32_t rows_in_this_strip = (i == num_strips - 1) ? (data.height - i * rows_per_strip) : rows_per_strip;
        size_t expected_uncompressed = (size_t)data.width * rows_in_this_strip * data.channels;
        size_t strip_capacity = (current_out_offset + expected_uncompressed > total_pixel_bytes) ? 
                                 (total_pixel_bytes - current_out_offset) : expected_uncompressed;

        if (counts[i] == 0 || offsets[i] == 0) {
            current_out_offset += expected_uncompressed;
            continue;
        }

        uint8_t *comp_buf = (uint8_t *)malloc(counts[i]);
        if (!comp_buf) break;

        fseek(file, offsets[i], SEEK_SET);
        if (fread(comp_buf, 1, counts[i], file) == counts[i]) {
            if (compression == 1) {
                size_t copy_size = (counts[i] < strip_capacity) ? counts[i] : strip_capacity;
                memcpy(data.pixel_bytes + current_out_offset, comp_buf, copy_size);
                current_out_offset += expected_uncompressed;
            } else if (compression == 5) {
                decompress_lzw_strip(comp_buf, counts[i], data.pixel_bytes + current_out_offset, strip_capacity);
                
                if (predictor == 2) {
                    uint8_t *strip_ptr = data.pixel_bytes + current_out_offset;
                    for (uint32_t r = 0; r < rows_in_this_strip; r++) {
                        uint8_t *row_ptr = strip_ptr + r * data.width * data.channels;
                        size_t row_bytes = data.width * data.channels;
                        for (size_t b = data.channels; b < row_bytes; b++) {
                            row_ptr[b] = (uint8_t)(row_ptr[b] + row_ptr[b - data.channels]);
                        }
                    }
                }
                current_out_offset += expected_uncompressed;
            }
        }
        free(comp_buf);
    }

    free(offsets);
    free(counts);
    fclose(file);

    data.data_size = total_pixel_bytes;
    data.is_valid = 1;
    return data;
}

void free_tiff_pixels(TIFFPixelData *data) {
    if (data && data->pixel_bytes) {
        free(data->pixel_bytes);
        data->pixel_bytes = NULL;
    }
}