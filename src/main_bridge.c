#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern PyObject* py_read_tiff_header(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_tags(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_format(PyObject* self, PyObject* args); // YENİ

static PyMethodDef OrganoidMethods[] = {
    {"read_header", py_read_tiff_header, METH_VARARGS, "Reads TIFF header report."},
    {"read_tags",   py_read_tiff_tags,   METH_VARARGS, "Reads TIFF dimensions report."},
    {"read_format", py_read_tiff_format, METH_VARARGS, "Reads TIFF format and color report."}, // YENİ
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