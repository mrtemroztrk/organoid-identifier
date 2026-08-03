#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "organoid_segmenter.h"

PyObject* py_segment_organoids(PyObject* self, PyObject* args) {
    Py_buffer view;
    int channels, width, height;
    int min_size = 50;
    double min_circularity = 0.10;

    if (!PyArg_ParseTuple(args, "y*iii|id", &view, &channels, &width, &height, &min_size, &min_circularity)) {
        return NULL;
    }

    int total_pixels = width * height;
    if (view.len < (Py_ssize_t)(total_pixels * channels)) {
        PyBuffer_Release(&view);
        PyErr_SetString(PyExc_ValueError, "Image buffer length is smaller than width * height * channels.");
        return NULL;
    }

    uint8_t* out_mask = (uint8_t*)malloc(total_pixels * sizeof(uint8_t));
    int32_t* out_labels = (int32_t*)malloc(total_pixels * sizeof(int32_t));

    if (!out_mask || !out_labels) {
        PyBuffer_Release(&view);
        if (out_mask) free(out_mask);
        if (out_labels) free(out_labels);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate segmentation output buffers.");
        return NULL;
    }

    int object_count = segment_organoids((const uint8_t*)view.buf, channels, width, height, min_size, min_circularity, out_mask, out_labels);
    PyBuffer_Release(&view);

    PyObject* mask_bytes_obj = PyBytes_FromStringAndSize((const char*)out_mask, total_pixels);
    PyObject* labels_bytes_obj = PyBytes_FromStringAndSize((const char*)out_labels, total_pixels * sizeof(int32_t));

    free(out_mask);
    free(out_labels);

    PyObject* dict = PyDict_New();
    PyDict_SetItemString(dict, "object_count", PyLong_FromLong(object_count));
    PyDict_SetItemString(dict, "mask_bytes", mask_bytes_obj);
    PyDict_SetItemString(dict, "labels_bytes", labels_bytes_obj);

    Py_DECREF(mask_bytes_obj);
    Py_DECREF(labels_bytes_obj);

    return dict;
}
