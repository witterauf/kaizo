#pragma once

#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyDataReader
{
    PyObject_HEAD;
    fuse::binary::DataReader reader;
};

extern PyTypeObject PyDataReaderType;

struct PyDataWriter
{
    PyObject_HEAD;
    fuse::binary::DataWriter writer;
};

extern PyTypeObject PyDataWriterType;

bool registerFuseBinary(PyObject* module);