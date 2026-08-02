#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "../tiff_header/tiff_header.h"
#include "tiff_format.h"

PyObject* py_read_tiff_format(PyObject* self, PyObject* args) {
    const char* file_path;
    if (!PyArg_ParseTuple(args, "s", &file_path)) return NULL;

    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF header.");
        return NULL;
    }

    TIFFFormatInfo info = read_tiff_format(file_path, header.ifd_offset);
    if (!info.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Failed to read TIFF format tags.");
        return NULL;
    }

    // Renk Modeli Metin Karşılığı
    const char* color_space_str = "Unknown";
    if (info.photometric_interp == 0 || info.photometric_interp == 1) {
        color_space_str = "Grayscale / Monochromatic";
    } else if (info.photometric_interp == 2) {
        color_space_str = "RGB Color";
    }

    // Sıkıştırma Metin Karşılığı
    const char* comp_str = "Unknown";
    if (info.compression == 1) comp_str = "Uncompressed (None)";
    else if (info.compression == 5) comp_str = "LZW Compression";

    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
        "\n┌───────────────────────────────────────────────────────┐\n"
        "│               TIFF FORMAT & COLOR REPORT              │\n"
        "├───────────────────────────────────────────────────────┤\n"
        "│  • Channels (Samples): %-30u │\n"
        "│  • Color Space       : %-30s │\n"
        "│  • Compression       : %-30s │\n"
        "└───────────────────────────────────────────────────────┘\n",
        info.samples_per_pixel > 0 ? info.samples_per_pixel : 1,
        color_space_str,
        comp_str
    );

    return PyUnicode_FromString(buffer);
}