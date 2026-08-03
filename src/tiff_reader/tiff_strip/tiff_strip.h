#ifndef TIFF_STRIP_H
#define TIFF_STRIP_H

#include <stdint.h>

/**
 * C Kavramı: struct (Yapı)
 * Farklı türdeki değişkenleri tek bir paket altında toplamaya yarar.
 */
typedef struct {
    uint32_t strip_offset;     // Tag 273: Resim piksellerinin başladığı bayt adresi
    uint32_t strip_byte_count; // Tag 279: Piksel verisinin toplam boyutu (bayt)
    int is_valid;              // Okuma başarılı mı? (1: Evet, 0: Hayır)
} TIFFStripInfo;

/**
 * Fonksiyon Bildirimi (Prototype)
 */
TIFFStripInfo read_tiff_strip(const char *file_path, uint32_t ifd_offset);

#endif // TIFF_STRIP_H