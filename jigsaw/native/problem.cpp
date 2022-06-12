
#define NO_IMPORT_ARRAY
#include "common.h"


struct ProblemObject {
    PyObject_HEAD;
    Problem * problem;
};


template <typename T> int as_dtype = NPY_NOTYPE;
template <> int as_dtype<int8_t> = NPY_INT8;
template <> int as_dtype<uint8_t> = NPY_UINT8;
template <> int as_dtype<int16_t> = NPY_INT16;
template <> int as_dtype<uint16_t> = NPY_UINT16;
template <> int as_dtype<int32_t> = NPY_INT32;
template <> int as_dtype<uint32_t> = NPY_UINT32;
template <> int as_dtype<int64_t> = NPY_INT64;
template <> int as_dtype<uint64_t> = NPY_UINT64;
template <> int as_dtype<float> = NPY_FLOAT;
template <> int as_dtype<double> = NPY_DOUBLE;


template <typename T>
static int array_converter(PyObject * object, PyArrayObject ** address) {

    // Clean up
    if (!object) {
        Py_XDECREF((PyObject *)*address);
        *address = nullptr;
        return 1;
    }

    // Get as integer array
    PyArrayObject * array = (PyArrayObject *)PyArray_FROM_OTF(object, as_dtype<T>, NPY_ARRAY_FORCECAST);
    if (!array)
        return 0;

    *address = array;
    return Py_CLEANUP_SUPPORTED;
}



static int Problem_init(ProblemObject * self, PyObject * args, PyObject * kwargs) {

    // Zero-initialize fields
    self->problem = nullptr;

    // Parse arguments
    PyArrayObject * pieces_array = nullptr;
    PyArrayObject * horizontal_array = nullptr;
    PyArrayObject * vertical_array = nullptr;
    static char * kwlist[] = {"pieces", "horizontal", "vertical", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&O&:__init__", kwlist, array_converter<uint8_t>, &pieces_array, array_converter<int8_t>, &horizontal_array, array_converter<int8_t>, &vertical_array)) {
        goto err;
    }

    // Check piece array shape
    if (PyArray_NDIM(pieces_array) != 2 || PyArray_SHAPE(pieces_array)[0] > 255 || PyArray_SHAPE(pieces_array)[1] != 4) {
        PyErr_SetString(PyExc_ValueError, "pieces array must be (N, 4), with N<256");
        goto err;
    }
    unsigned N = (unsigned)PyArray_SHAPE(pieces_array)[0];

    // Check horizontal constraint array shape
    if (PyArray_NDIM(horizontal_array) != 2 || PyArray_SHAPE(horizontal_array)[0] < 1 || PyArray_SHAPE(horizontal_array)[1] < 2) {
        PyErr_SetString(PyExc_ValueError, "horizontal array must be (H, W + 1), with H>0 and W>0");
        goto err;
    }
    unsigned H = (unsigned)PyArray_SHAPE(horizontal_array)[0];
    unsigned W = (unsigned)PyArray_SHAPE(horizontal_array)[1] - 1;

    // Check vertical constraint array shape
    if (PyArray_NDIM(vertical_array) != 2 || PyArray_SHAPE(vertical_array)[0] != H + 1 || PyArray_SHAPE(vertical_array)[1] != W) {
        PyErr_SetString(PyExc_ValueError, "vertical array must be (H + 1, W), with H>0 and W>0");
        goto err;
    }

    // Create definition
    {
        Definition definition(H, W);

        // Add pieces
        for (unsigned i = 0; i < N; ++i) {
            Piece p;
            for (unsigned j = 0; j < 4; ++j)
                p.edges[j] = *(uint8_t *)PyArray_GETPTR2(pieces_array, i, j);
            definition.pieces[p] += 1;
        }

        // Add horizontal constraints
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x <= W; ++x)
                definition.horizontal_constraint(y, x) = *(int8_t *)PyArray_GETPTR2(horizontal_array, y, x);

        // Add vertical constraints
        for (unsigned y = 0; y <= H; ++y)
            for (unsigned x = 0; x < W; ++x)
                definition.horizontal_constraint(y, x) = *(int8_t *)PyArray_GETPTR2(vertical_array, y, x);

        // Release arrays
        Py_XDECREF(pieces_array);
        Py_XDECREF(horizontal_array);
        Py_XDECREF(vertical_array);

        // Make problem
        try {
            self->problem = new Problem(definition);
        } catch (std::bad_alloc &) {
            PyErr_NoMemory();
            return -1;
        }
    }
    return 0;

err:
    Py_XDECREF(pieces_array);
    Py_XDECREF(horizontal_array);
    Py_XDECREF(vertical_array);
    return -1;
}


static void Problem_dealloc(ProblemObject * self) {
    delete self->problem;
    Py_TYPE(self)->tp_free((PyObject *)self);
}


static PyObject * Problem_iter(PyObject * self) {
    Py_INCREF(self);
    return self;
}


static PyObject * Problem_iternext(ProblemObject * self) {

    // Iterator is already exhausted
    if (!self->problem)
        return nullptr;

    // Find next solution, if any
    bool has;
    Py_BEGIN_ALLOW_THREADS
    has = self->problem->next();
    Py_END_ALLOW_THREADS

    // Is the iterator exhausted?
    if (!has) {
        delete self->problem;
        self->problem = nullptr;
        return nullptr;
    }

    // Wrap solution as array
    unsigned H = self->problem->H;
    unsigned W = self->problem->W;
    npy_intp dims[3] = {H, W, 4};
    PyObject * object = PyArray_SimpleNew(3, dims, NPY_UINT8);
    if (!object)
        return nullptr;
    for (unsigned y = 0; y < self->problem->H; ++y)
        for (unsigned x = 0; x < self->problem->W; ++x) {
            Piece p = self->problem->pieces[self->problem->stack[(y + 1) * W + x]];
            for (unsigned i = 0; i < 4; ++i)
                *(uint8_t *)PyArray_GETPTR3((PyArrayObject *)object, y, x, i) = p.edges[i];
        }
    return object;
}


PyTypeObject Problem_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /* .tp_name = */ "jigsaw.native.Problem",
    /* .tp_basicsize = */ sizeof(ProblemObject),
    /* .tp_itemsize = */ 0,
    /* .tp_dealloc = */ (destructor)Problem_dealloc,
    /* .tp_print = */ 0,
    /* .tp_getattr = */ 0,
    /* .tp_setattr = */ 0,
    /* .tp_reserved = */ 0,
    /* .tp_repr = */ 0,
    /* .tp_as_number = */ 0,
    /* .tp_as_sequence = */ 0,
    /* .tp_as_mapping = */ 0,
    /* .tp_hash = */ 0,
    /* .tp_call = */ 0,
    /* .tp_str = */ 0,
    /* .tp_getattro = */ 0,
    /* .tp_setattro = */ 0,
    /* .tp_as_buffer = */ 0,
    /* .tp_flags = */ Py_TPFLAGS_DEFAULT,
    /* .tp_doc = */ PyDoc_STR(""),
    /* .tp_traverse = */ 0,
    /* .tp_clear = */ 0,
    /* .tp_richcompare = */ 0,
    /* .tp_weaklistoffset = */ 0,
    /* .tp_iter = */ Problem_iter,
    /* .tp_iternext = */ (iternextfunc)Problem_iternext,
    /* .tp_methods = */ 0,
    /* .tp_members = */ 0,
    /* .tp_getset = */ 0,
    /* .tp_base = */ 0,
    /* .tp_dict = */ 0,
    /* .tp_descr_get = */ 0,
    /* .tp_descr_set = */ 0,
    /* .tp_dictoffset = */ 0,
    /* .tp_init = */ (initproc)Problem_init,
    /* .tp_alloc  = */ 0,
    /* .tp_new  = */ PyType_GenericNew,
};
