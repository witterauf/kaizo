#pragma once

#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/PointerData.h>
#include <fuse/binary/data/RecordData.h>
#include <fuse/binary/data/StringData.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

auto toNativePython(const fuse::binary::Data& data) -> PyObject*;
auto toNativePython(const fuse::binary::IntegerData& data) -> PyObject*;
auto toNativePython(const fuse::binary::StringData& data) -> PyObject*;
auto toNativePython(const fuse::binary::ArrayData& array) -> PyObject*;
auto toNativePython(const fuse::binary::RecordData& record) -> PyObject*;

auto fromNativePython(PyObject* data) -> std::unique_ptr<fuse::binary::Data>;

/*


struct PyData
{
    PyObject_HEAD;
    bool ownsData;
    fuse::binary::Data* data;
};

struct PyIntegerData
{
    PyData base;
};

struct PyStringData
{
    PyData base;
};

struct PyRecordData
{
    PyData base;
};

struct PyArrayData
{
    PyData base;
};

struct PyPointerData
{
    PyData base;
};

bool registerFuseData(PyObject* module);
*/