#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../tiff_header/tiff_header.h"
#include "tiff_pixels.h"

PyObject* py_read_tiff_pixels(PyObject* self, PyObject* args) {
    const char* file_path;

    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    TIFFHeader header = read_tiff_header(file_path);
    if (!header.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Invalid TIFF Header");
        return NULL;
    }

    TIFFPixelData pixels = read_tiff_pixels(file_path, header.ifd_offset);
    if (!pixels.is_valid) {
        PyErr_SetString(PyExc_ValueError, "Failed to read TIFF pixel payload");
        return NULL;
    }

    PyObject* py_bytes = PyBytes_FromStringAndSize((const char*)pixels.pixel_bytes, pixels.data_size);
    free_tiff_pixels(&pixels);

    return Py_BuildValue("{s:i, s:i, s:i, s:O}",
        "width", pixels.width,
        "height", pixels.height,
        "channels", pixels.channels,
        "raw_bytes", py_bytes
    );
}