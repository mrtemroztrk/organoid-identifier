#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "tiff_header.h"

PyObject* py_read_tiff_header(PyObject* self, PyObject* args) {
    const char* file_path;
    if (!PyArg_ParseTuple(args, "s", &file_path)) return NULL;

    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF header.");
        return NULL;
    }

    const char* endian_str = (header.byte_order == 0x4949) ? "Little-Endian (II)" : "Big-Endian (MM)";
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
        "\n┌───────────────────────────────────────────────────────┐\n"
        "│              TIFF FILE INSPECTION REPORT              │\n"
        "├───────────────────────────────────────────────────────┤\n"
        "│  • File Path         : %-30s │\n"
        "│  • Status            : %-30s │\n"
        "│  • Byte Order        : %-30s │\n"
        "│  • Magic Number      : %-30d │\n"
        "│  • First IFD Offset  : %-24u bytes │\n"
        "└───────────────────────────────────────────────────────┘\n",
        file_path, "VALID TIFF", endian_str, header.magic_number, header.ifd_offset
    );

    return PyUnicode_FromString(buffer);
}