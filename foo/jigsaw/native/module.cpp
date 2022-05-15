
#include "common.h"


extern PyTypeObject Problem_Type;


static PyMethodDef methods[] = {
    {NULL},
};


static PyTypeObject * types[] = {
    &Problem_Type,
    NULL,
};


static int module_exec(PyObject * module) {

    // Initialize types
    for (PyTypeObject ** t = types; *t; ++t) {
        if (PyType_Ready(*t) < 0) {
            goto fail;
        }
    }

    // Register types
    for (PyTypeObject ** t = types; *t; ++t) {
        Py_INCREF(*t);
        if (PyModule_AddObject(module, (*t)->tp_name + sizeof("jigsaw.native.") - 1, (PyObject *)*t) < 0) {
            Py_DECREF(*t);
            goto fail;
        }
    }

    return 0;

fail:
    Py_XDECREF(module);
    return -1;
}


static PyModuleDef_Slot slots[] = {
    {Py_mod_exec, (void *)module_exec},
    {0, NULL},
};


static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    /* .m_name = */ "core_parser.native",
    /* .m_doc = */ NULL,
    /* .m_size = */ 0,
    /* .m_methods = */ methods,
    /* .m_slots = */ slots,
};


PyMODINIT_FUNC PyInit_native() {

    // Initialize NumPy
    import_array();
    if (PyErr_Occurred()) {
        return NULL;
    }

    // Initialize module
    return PyModuleDef_Init(&module);
}
