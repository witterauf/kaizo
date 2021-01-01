#pragma once

#include <fuse/linking/Backtracker.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyBacktrackingPacker
{
    PyObject_HEAD;
    fuse::BacktrackingPacker* packer;
};

extern PyTypeObject PyBacktrackingPackerType;

bool registerLinkingTypes(PyObject* module);