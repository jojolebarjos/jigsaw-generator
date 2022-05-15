
#define NO_IMPORT_ARRAY
#include "common.h"

#include <vector>
#include <random>

#include <cstdio>


/*
static int grid_converter(PyObject * object, void * address) {

    // Extract as NumPy array
    PyArrayObject * array = (PyArrayObject *)PyArray_FROM_OT(object, NPY_UINT8);
    if (!array)
        return 0;

    // This should be a HxWx4 array
    if (PyArray_NDIM(array) != 3
}
*/


static int vector_converter(PyObject * object, void * address) {

    // Extract as NumPy array
    PyArrayObject * array = (PyArrayObject *)PyArray_FROM_OT(object, NPY_UINT8);
    if (!array)
        return 0;

    // This should be a 1D array, with at least one element
    if (PyArray_NDIM(array) != 1) {
        PyErr_SetString(PyExc_ValueError, "1-D array expected");
        return 0;
    }
    if (PyArray_DIM(array, 0) <= 0) {
        PyErr_SetString(PyExc_ValueError, "at least one element is expected");
        return 0;
    }

    // Fill vector
    std::vector<unsigned> & values = *(std::vector<unsigned> *)address;
    npy_intp dim = PyArray_DIM(array, 0);
    values.resize(dim);
    for (unsigned i = 0; i < dim; ++i)
        values[i] = *(uint8_t *)PyArray_GETPTR1(array, i);
    return 1;
}


static PyObject * build_array(Grid const & grid) {

    npy_intp dims[3] = {grid.H, grid.W, 4};
    PyObject * object = PyArray_SimpleNew(3, dims, NPY_UINT8);
    if (!object)
        return nullptr;

    for (unsigned y = 0; y < grid.H; ++y)
        for (unsigned x = 0; x < grid.W; ++x) {
            Piece p = grid.at(y, x);
            for (unsigned i = 0; i < 4; ++i)
                *(uint8_t *)PyArray_GETPTR3((PyArrayObject *)object, y, x, i) = p.edges[i];
        }

    return object;
}



PyObject * Native_generate(PyObject * self, PyObject * args) {

    // Parse arguments
    unsigned H, W;
    unsigned seed = 1;
    std::vector<uint8_t> edges;
    if (!PyArg_ParseTuple(args, "IIO&|I:generate", &H, &W, vector_converter, &edges, &seed))
        return nullptr;

    // Create sampler
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<unsigned> distribution(0, edges.size() - 1);
    auto sampler = [&]() { return edges[distribution(generator)]; };

    // Loop forever
    while (true) {

        // Generate random board, where all pieces are unique
        Grid grid(H, W);
        do {
            grid.randomize(sampler);
            // TODO add an option
        } while (grid.has_duplicate());

        // Create solver
        Definition definition = grid.to_definition();
        Problem problem(definition);

        // Count solutions
        // Note: by design, more than 8 solutions means that it is not unique
        unsigned count = 0;
        while (count < 9 && problem.next())
            ++count;
        if (count > 8)
            continue;
        // TODO make sure that having <=8 indeed means it is unique

        // Convert to array
        return build_array(grid);
    }

}

