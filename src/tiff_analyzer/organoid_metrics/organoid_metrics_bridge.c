#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "organoid_metrics.h"

PyObject* py_calculate_organoid_metrics(PyObject* self, PyObject* args) {
    Py_buffer mask_view;
    Py_buffer img_view;
    int width, height;
    int channels = 1;

    PyObject* img_obj = Py_None;

    if (!PyArg_ParseTuple(args, "y*ii|Oi", &mask_view, &width, &height, &img_obj, &channels)) {
        return NULL;
    }

    if (mask_view.len < (Py_ssize_t)(width * height)) {
        PyBuffer_Release(&mask_view);
        PyErr_SetString(PyExc_ValueError, "Mask buffer length is smaller than width * height.");
        return NULL;
    }

    const uint8_t* img_bytes = NULL;
    if (img_obj != Py_None) {
        if (PyObject_GetBuffer(img_obj, &img_view, PyBUF_SIMPLE) == 0) {
            img_bytes = (const uint8_t*)img_view.buf;
        }
    }

    OrganoidMetricResult result;
    int count = calculate_organoid_metrics((const uint8_t*)mask_view.buf, img_bytes, channels, width, height, &result, 1);

    if (img_bytes) {
        PyBuffer_Release(&img_view);
    }
    PyBuffer_Release(&mask_view);

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

    // Signal Intensity Metrics (Pixel-wise)
    PyDict_SetItemString(dict, "mean_intensity", PyFloat_FromDouble(result.mean_intensity));
    PyDict_SetItemString(dict, "integrated_intensity", PyFloat_FromDouble(result.integrated_intensity));
    PyDict_SetItemString(dict, "min_intensity", PyFloat_FromDouble(result.min_intensity));
    PyDict_SetItemString(dict, "max_intensity", PyFloat_FromDouble(result.max_intensity));
    PyDict_SetItemString(dict, "std_intensity", PyFloat_FromDouble(result.std_intensity));

    return dict;
}

PyObject* py_calculate_multi_object_metrics(PyObject* self, PyObject* args) {
    Py_buffer labels_view;
    Py_buffer img_view;
    int num_objects, width, height;
    int channels = 1;
    PyObject* img_obj = Py_None;

    if (!PyArg_ParseTuple(args, "y*iii|Oi", &labels_view, &num_objects, &width, &height, &img_obj, &channels)) {
        return NULL;
    }

    if (labels_view.len < (Py_ssize_t)(width * height * sizeof(int32_t))) {
        PyBuffer_Release(&labels_view);
        PyErr_SetString(PyExc_ValueError, "Labels buffer length is smaller than width * height * sizeof(int32_t).");
        return NULL;
    }

    const uint8_t* img_bytes = NULL;
    if (img_obj != Py_None) {
        if (PyObject_GetBuffer(img_obj, &img_view, PyBUF_SIMPLE) == 0) {
            img_bytes = (const uint8_t*)img_view.buf;
        }
    }

    OrganoidMetricResult* results = (OrganoidMetricResult*)malloc(num_objects * sizeof(OrganoidMetricResult));
    if (!results) {
        if (img_bytes) PyBuffer_Release(&img_view);
        PyBuffer_Release(&labels_view);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for multi-object metric results.");
        return NULL;
    }

    int processed = calculate_multi_object_metrics(
        (const int32_t*)labels_view.buf,
        img_bytes,
        channels,
        width,
        height,
        num_objects,
        results
    );

    if (img_bytes) PyBuffer_Release(&img_view);
    PyBuffer_Release(&labels_view);

    PyObject* list = PyList_New(0);
    for (int k = 0; k < processed; k++) {
        PyObject* dict = PyDict_New();
        PyDict_SetItemString(dict, "id", PyLong_FromLong(results[k].label_id));
        PyDict_SetItemString(dict, "area", PyLong_FromSize_t(results[k].area));
        PyDict_SetItemString(dict, "perimeter", PyLong_FromSize_t(results[k].perimeter));
        PyDict_SetItemString(dict, "circularity", PyFloat_FromDouble(results[k].circularity));
        PyDict_SetItemString(dict, "equivalent_diameter", PyFloat_FromDouble(results[k].equiv_diameter));

        PyObject* centroid = PyTuple_Pack(2, PyFloat_FromDouble(results[k].centroid_x), PyFloat_FromDouble(results[k].centroid_y));
        PyDict_SetItemString(dict, "centroid", centroid);
        Py_DECREF(centroid);

        PyObject* bbox = PyTuple_Pack(4, PyLong_FromLong(results[k].min_x), PyLong_FromLong(results[k].min_y), PyLong_FromLong(results[k].max_x), PyLong_FromLong(results[k].max_y));
        PyDict_SetItemString(dict, "bbox", bbox);
        Py_DECREF(bbox);

        PyDict_SetItemString(dict, "mean_intensity", PyFloat_FromDouble(results[k].mean_intensity));
        PyDict_SetItemString(dict, "integrated_intensity", PyFloat_FromDouble(results[k].integrated_intensity));
        PyDict_SetItemString(dict, "min_intensity", PyFloat_FromDouble(results[k].min_intensity));
        PyDict_SetItemString(dict, "max_intensity", PyFloat_FromDouble(results[k].max_intensity));
        PyDict_SetItemString(dict, "std_intensity", PyFloat_FromDouble(results[k].std_intensity));

        PyList_Append(list, dict);
        Py_DECREF(dict);
    }

    free(results);
    return list;
}
