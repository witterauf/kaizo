#pragma once

#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/AddressStorageFormat.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyAddressFormat
{
    PyObject_HEAD;
    const fuse::AddressFormat* format;
};

struct PyAddressLayout
{
    PyObject_HEAD;
    fuse::AddressStorageFormat* layout;
};

struct PyRelativeAddressLayout
{
    PyAddressLayout base;
};

extern PyTypeObject PyAddressFormatType;
extern PyTypeObject PyAddressLayoutType;
extern PyTypeObject PyRelativeAddressLayoutType;

bool registerFuseAddresses(PyObject* module);