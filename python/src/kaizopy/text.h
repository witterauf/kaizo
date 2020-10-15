#pragma once

#include <kaizo/text/Table.h>

#ifdef _WIN32
#ifdef WIN_EXPORT
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED __declspec(dllimport)
#endif
#else
#define EXPORTED
#endif

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyKaizoTable
{
    PyObject_HEAD;
    kaizo::text::Table* table;
};

extern EXPORTED PyTypeObject PyKaizoTableType;
auto EXPORTED makeTable(kaizo::text::Table* table) -> PyKaizoTable*;

bool registerKaizoText(PyObject* module);
}