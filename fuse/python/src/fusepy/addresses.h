#pragma once

#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/AddressMap.h>
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

struct PyMipsEmbeddedLayout
{
    PyAddressLayout base;
};

struct PyAddressMap
{
    PyObject_HEAD;
    fuse::AddressMap* map;
};

struct PyRegionedAddressMap
{
    PyAddressMap base;
};

struct PyFuseAddress
{
    PyObject_HEAD;
    fuse::Address address;
};

extern PyTypeObject PyAddressFormatType;
extern PyTypeObject PyAddressLayoutType;
extern PyTypeObject PyRelativeAddressLayoutType;
extern PyTypeObject PyMipsEmbeddedLayoutType;
extern PyTypeObject PyAddressMapType;
extern PyTypeObject PyRegionedAddressMapType;
extern PyTypeObject PyFuseAddressType;

bool registerFuseAddresses(PyObject* module);

auto PyAddressLayout_New(std::unique_ptr<fuse::AddressStorageFormat>&& layout) -> PyObject*;