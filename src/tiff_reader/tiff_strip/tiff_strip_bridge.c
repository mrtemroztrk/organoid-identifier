#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "../tiff_header/tiff_header.h"
#include "tiff_strip.h"

PyObject* py_read_tiff_strip(PyObject* self, PyObject* args) {
    const char* file_path;

    // 1. Python'dan gelen str argümanını C string'ine (const char*) çevir
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    // 2. Önce Header okuyup IFD adresini öğrenelim
    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF Header");
        return NULL;
    }

    // 3. Strip verisini okuyalım
    TIFFStripInfo info = read_tiff_strip(file_path, header.ifd_offset);
    if (!info.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Failed to read Strip metadata");
        return NULL;
    }

    // 4. Şık bir terminal raporu metni oluşturalım
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
        "\n┌───────────────────────────────────────────────────────┐\n"
        "│              TIFF STRIP & PIXEL LOCATION              │\n"
        "├───────────────────────────────────────────────────────┤\n"
        "│  • Strip Offset      : %-10u bytes (Address) │\n"
        "│  • Raw Pixel Data    : %-10u bytes (Length)  │\n"
        "└───────────────────────────────────────────────────────┘\n",
        info.strip_offset,
        info.strip_byte_count
    );

    // 5. C string'ini Python 'str' (PyUnicode) nesnesine dönüştürüp döndür
    return PyUnicode_FromString(buffer);
}