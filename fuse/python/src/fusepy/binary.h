#pragma once

#include <fuse/binary/DataReader.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyDataReader
{
    PyObject_HEAD;
    fuse::binary::DataReader reader;
};

extern PyTypeObject PyDataReaderType;

bool registerFuseBinary(PyObject* module);