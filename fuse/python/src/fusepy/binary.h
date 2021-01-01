#pragma once

#include <fuse/BinaryPatch.h>
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

struct PyFuseBinaryPatch
{
    PyObject_HEAD;
    fuse::BinaryPatch patch;
};

extern PyTypeObject PyFuseBinaryPatchType;

auto PyFuseBinaryPatch_New(const uint64_t data, const uint64_t mask, const size_t size,
                           const ptrdiff_t offset) -> PyObject*;
auto PyFuseBinaryPatch_New(const fuse::BinaryPatch& patch) -> PyObject*;

bool registerFuseBinary(PyObject* module);