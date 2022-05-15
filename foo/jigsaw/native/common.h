#ifndef NATIVE_COMMON_H
#define NATIVE_COMMON_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL native_ARRAY_API
#include <numpy/arrayobject.h>

#include "../../include/piece.h"
#include "../../include/problem.h"
#include "../../include/grid.h"


#endif
