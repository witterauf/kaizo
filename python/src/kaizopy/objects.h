#pragma once

#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/binary/objects/Object.h>
#include <fuse/binary/objects/UnresolvedReference.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

auto toPython(const fuse::AnnotatedBinary& binary) -> PyObject*;
auto toPython(const fuse::Object& object) -> PyObject*;
auto toPython(const fuse::UnresolvedReference& ref) -> PyObject*;