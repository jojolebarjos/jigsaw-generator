
#define NO_IMPORT_ARRAY
#include "common.h"


struct ProblemObject {
    PyObject_HEAD;
    Problem * problem;
};


static int pieces_converter(PyObject * object, std::vector<Piece> * address) {

    // Get as integer array
    PyArrayObject * array = (PyArrayObject *)PyArray_FROM_OTF(object, NPY_UINT8, NPY_ARRAY_FORCECAST);
    if (!array)
        return 0;

    // Must be N x 4
    if (PyArray_NDIM(array) != 2 || PyArray_SHAPE(array)[1] != 4) {
        Py_DECREF(array);
        PyErr_SetString(PyExc_ValueError, "array shape must be (N, 4)");
        return 0;
    }
    npy_intp N = PyArray_SHAPE(array)[0];

    // Populate vector
    std::vector<Piece> & pieces = *address;
    pieces.resize(N);
    Piece p;
    for (npy_intp i = 0; i < N; ++i) {
        for (unsigned j = 0; j < 4; ++j)
            p.edges[j] = *(uint8_t *)PyArray_GETPTR2(array, i, j);
        pieces[i] = p;
    }
    Py_DECREF(array);

    return 1;
}


static int Problem_init(ProblemObject * self, PyObject * args, PyObject * kwargs) {

    // Zero-initialize fields
    self->problem = nullptr;
    
    // Expect a single argument
    unsigned H, W;
    std::vector<Piece> pieces;
    // TODO constraints
    static char * kwlist[] = {"height", "width", "pieces", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IIO&:__init__", kwlist, &H, &W, pieces_converter, &pieces))
        return -1;

    // Make definition
    Definition definition(H, W);
    for (Piece p : pieces)
        definition.pieces[p] += 1;

    // Make problem
    try {
        self->problem = new Problem(definition);
    } catch (std::bad_alloc &) {
        PyErr_NoMemory();
        return -1;
    }
    return 0;
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
