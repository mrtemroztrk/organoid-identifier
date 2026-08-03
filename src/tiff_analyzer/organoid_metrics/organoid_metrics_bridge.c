#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "organoid_metrics.h"

PyObject* py_calculate_organoid_metrics(PyObject* self, PyObject* args) {
    Py_buffer view;
    int width, height;

    if (!PyArg_ParseTuple(args, "y*ii", &view, &width, &height)) {
        return NULL;
    }

    if (view.len < (Py_ssize_t)(width * height)) {
        PyBuffer_Release(&view);
        PyErr_SetString(PyExc_ValueError, "Mask buffer length is smaller than width * height.");
        return NULL;
    }

    OrganoidMetricResult result;
    int count = calculate_organoid_metrics((const uint8_t*)view.buf, width, height, &result, 1);
    PyBuffer_Release(&view);

    if (count == 0) {
        Py_RETURN_NONE;
    }

    PyObject* dict = PyDict_New();
    PyDict_SetItemString(dict, "area", PyLong_FromSize_t(result.area));
    PyDict_SetItemString(dict, "perimeter", PyLong_FromSize_t(result.perimeter));
    PyDict_SetItemString(dict, "circularity", PyFloat_FromDouble(result.circularity));
    PyDict_SetItemString(dict, "equivalent_diameter", PyFloat_FromDouble(result.equiv_diameter));

    PyObject* centroid = PyTuple_Pack(2, PyFloat_FromDouble(result.centroid_x), PyFloat_FromDouble(result.centroid_y));
    PyDict_SetItemString(dict, "centroid", centroid);
    Py_DECREF(centroid);

    PyObject* bbox = PyTuple_Pack(4, PyLong_FromLong(result.min_x), PyLong_FromLong(result.min_y), PyLong_FromLong(result.max_x), PyLong_FromLong(result.max_y));
    PyDict_SetItemString(dict, "bbox", bbox);
    Py_DECREF(bbox);

    return dict;
}
