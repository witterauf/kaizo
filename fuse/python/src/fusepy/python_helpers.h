#pragma once

#include <optional>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

auto splitKeywordArgs(PyObject* kwargs, const char** keywords)
    -> std::optional<std::pair<PyObject*, PyObject*>>;
