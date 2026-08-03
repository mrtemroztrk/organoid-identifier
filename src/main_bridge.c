#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern PyObject* py_read_tiff_header(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_tags(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_format(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_strip(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_bits(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_pixels(PyObject* self, PyObject* args);
extern PyObject* py_calculate_organoid_metrics(PyObject* self, PyObject* args);
extern PyObject* py_calculate_multi_object_metrics(PyObject* self, PyObject* args);
extern PyObject* py_segment_organoids(PyObject* self, PyObject* args);

static PyMethodDef OrganoidMethods[] = {
    {"read_header", py_read_tiff_header, METH_VARARGS, "Reads TIFF header report."},
    {"read_tags",   py_read_tiff_tags,   METH_VARARGS, "Reads TIFF dimensions report."},
    {"read_format", py_read_tiff_format, METH_VARARGS, "Reads TIFF format report."},
    {"read_strip",  py_read_tiff_strip,  METH_VARARGS, "Reads TIFF pixel location report."},
    {"read_bits",   py_read_tiff_bits,   METH_VARARGS, "Reads TIFF bit depth report."},
    {"read_pixels", py_read_tiff_pixels, METH_VARARGS, "Extracts raw pixel payload."},
    {"calculate_metrics", py_calculate_organoid_metrics, METH_VARARGS, "Calculates morphometric metrics for organoid mask."},
    {"calculate_multi_metrics", py_calculate_multi_object_metrics, METH_VARARGS, "Calculates metrics for multiple labeled organoid objects."},
    {"segment_organoids", py_segment_organoids, METH_VARARGS, "Performs connected component segmentation on organoid image."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef organoidmodule = {
    PyModuleDef_HEAD_INIT,
    "_core",
    "Organoid Identifier Core C Extension",
    -1,
    OrganoidMethods
};

PyMODINIT_FUNC PyInit__core(void) {
    return PyModule_Create(&organoidmodule);
}