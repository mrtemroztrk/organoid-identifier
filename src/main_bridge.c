#define PY_SSIZE_T_CLEAN
#include <Python.h>

// İzole bridge dosyalarımızdan gelen fonksiyon prototipleri
extern PyObject* py_read_tiff_header(PyObject* self, PyObject* args);
extern PyObject* py_read_tiff_tags(PyObject* self, PyObject* args);

// Modüllerden gelen bağımsız fonksiyonların Python tablosu
static PyMethodDef OrganoidMethods[] = {
    {"read_header", py_read_tiff_header, METH_VARARGS, "Reads TIFF header report."},
    {"read_tags",   py_read_tiff_tags,   METH_VARARGS, "Reads TIFF dimensions and tags report."},
    {NULL, NULL, 0, NULL}
};

// Modül tanımı
static struct PyModuleDef organoidmodule = {
    PyModuleDef_HEAD_INIT,
    "_core",
    "Organoid Identifier Core C Extension",
    -1,
    OrganoidMethods
};

// Python başlatma fonksiyonu
PyMODINIT_FUNC PyInit__core(void) {
    return PyModule_Create(&organoidmodule);
}