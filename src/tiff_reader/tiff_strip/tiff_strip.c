#include <stdio.h>
#include "tiff_strip.h"

TIFFStripInfo read_tiff_strip(const char *file_path, uint32_t ifd_offset) {
    // 1. Yapı değişkenimizi sıfırlayarak başlatıyoruz
    TIFFStripInfo info = {0, 0, 0};

    // 2. C'de Dosya Açma: "rb" -> Read Binary (İkili modda okuma)
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return info; // Dosya açılamadıysa is_valid = 0 olan boş yapıyı dön
    }

    // 3. fseek: Dosya imlecini (cursor) doğrudan IFD adresine zıplatır
    if (fseek(file, ifd_offset, SEEK_SET) != 0) {
        fclose(file); // C'de açılan her dosya kapatılmak ZORUNDADIR!
        return info;
    }

    // 4. IFD'deki etiket (tag) sayısını okuyoruz (2 bayt)
    uint16_t tag_count = 0;
    if (fread(&tag_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return info;
    }

    // 5. Etiketleri döngüyle tarıyoruz (Her etiket 12 bayttır)
    for (uint16_t i = 0; i < tag_count; i++) {
        uint16_t tag_id;
        uint16_t tag_type;
        uint32_t tag_count_val;
        uint32_t tag_value;

        // 12 baytlık etiket yapısını sırayla oku
        if (fread(&tag_id, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_type, sizeof(uint16_t), 1, file) != 1) break;
        if (fread(&tag_count_val, sizeof(uint32_t), 1, file) != 1) break;
        if (fread(&tag_value, sizeof(uint32_t), 1, file) != 1) break;

        // Tag 273: StripOffsets (Piksel Verisinin Başlangıcı)
        if (tag_id == 273) {
            info.strip_offset = tag_value;
        }
        // Tag 279: StripByteCounts (Piksel Verisinin Boyutu)
        else if (tag_id == 279) {
            info.strip_byte_count = tag_value;
        }
    }

    // 6. Bellek temizliği ve başarılı dönüş
    fclose(file);
    info.is_valid = 1;
    return info;
}