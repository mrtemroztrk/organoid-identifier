#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "../tiff_header/tiff_header.h"
#include "tiff_tags.h"

/**
 * py_read_tiff_tags
 * -----------------
 * Python C API wrapper for reading TIFF tags and dimensions.
 */
PyObject* py_read_tiff_tags(PyObject* self, PyObject* args) {
    const char* file_path;

    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    // Önce başlığı okuyup IFD offset adresini alıyoruz
    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF header or file could not be opened.");
        return NULL;
    }

    // IFD etiketlerini okuyoruz
    TIFFImageInfo info = read_tiff_tags(file_path, header.ifd_offset);
    if (!info.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Failed to read TIFF tags from file.");
        return NULL;
    }

    // C seviyesinde biçimlendirilmiş rapor metni hazırlıyoruz
    char report_buffer[1024];
    snprintf(report_buffer, sizeof(report_buffer),
        "\n┌───────────────────────────────────────────────────────┐\n"
        "│               TIFF IMAGE DIMENSIONS REPORT            │\n"
        "├───────────────────────────────────────────────────────┤\n"
        "│  • Width  (Pixels)   : %-30u │\n"
        "│  • Height (Pixels)   : %-30u │\n"
        "│  • Bits Per Sample   : %-30u │\n"
        "└───────────────────────────────────────────────────────┘\n",
        info.width, info.height, info.bits_per_sample
    );

    return PyUnicode_FromString(report_buffer);
}