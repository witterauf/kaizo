#pragma once

#include <fusepy/Text.h>
#include <kaizo/text/Table.h>

#ifdef _WIN32
#ifdef KAIZO_WIN_EXPORT
#define KAIZO_EXPORTED __declspec(dllexport)
#else
#define KAIZO_EXPORTED __declspec(dllimport)
#endif
#else
#define KAIZO_EXPORTED
#endif

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyKaizoTable
{
    PyObject_HEAD;
    kaizo::text::Table* table;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoTableType;
auto KAIZO_EXPORTED makeTable(kaizo::text::Table* table) -> PyKaizoTable*;

struct PyKaizoTableEncoding
{
    PyTextEncoding base;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoTableEncodingType;

bool registerKaizoText(PyObject* module);
}