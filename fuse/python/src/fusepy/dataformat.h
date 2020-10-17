#pragma once

#include <fuse/binary/formats/ArrayFormat.h>
#include <fuse/binary/formats/DataFormat.h>
#include <fuse/binary/formats/IntegerFormat.h>
#include <fuse/binary/formats/PointerFormat.h>
#include <fuse/binary/formats/RecordFormat.h>
#include <fuse/binary/formats/StringFormat.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyDataFormat
{
    PyObject_HEAD;
    fuse::binary::DataFormat* format;
};

struct PyIntegerFormat
{
    PyDataFormat dataFormat;
};

struct PyStringFormat
{
    PyDataFormat dataFormat;
};

struct PyRecordFormat
{
    PyDataFormat dataFormat;
};

struct PyArrayFormat
{
    PyDataFormat dataFormat;
};

struct PyPointerFormat
{
    PyDataFormat dataFormat;
};

extern PyTypeObject PyDataFormatType;
extern PyTypeObject PyIntegerFormatType;
extern PyTypeObject PyStringFormatType;
extern PyTypeObject PyRecordFormatType;
extern PyTypeObject PyArrayFormatType;
extern PyTypeObject PyPointerFormatType;

bool registerDataFormatTypes(PyObject* module);