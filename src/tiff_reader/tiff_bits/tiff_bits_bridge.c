#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "../tiff_header/tiff_header.h"
#include "tiff_bits.h"

PyObject* py_read_tiff_bits(PyObject* self, PyObject* args) {
    const char* file_path;

    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF Header");
        return NULL;
    }

    TIFFBitsInfo info = read_tiff_bits(file_path, header.ifd_offset);
    if (!info.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Failed to read Bits metadata");
        return NULL;
    }

    const char* format_str = "Unsigned Integer";
    if (info.sample_format == 2) format_str = "Signed Integer";
    else if (info.sample_format == 3) format_str = "IEEE Floating Point";
    else if (info.sample_format == 4) format_str = "Undefined/Raw Data";

    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
        "\n┌───────────────────────────────────────────────────────┐\n"
        "│             TIFF BIT DEPTH & DATA TYPE                │\n"
        "├───────────────────────────────────────────────────────┤\n"
        "│  • Bits Per Sample   : %-2u bits                      │\n"
        "│  • Data Format       : %-25s │\n"
        "└───────────────────────────────────────────────────────┘\n",
        info.bits_per_sample,
        format_str
    );

    return PyUnicode_FromString(buffer);
}