#pragma once

#include <fuse/text/TextEncoding.h>

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

struct PyTextEncoding
{
    PyObject_HEAD;
    std::shared_ptr<fuse::text::TextEncoding> encoding;
};

extern EXPORTED PyTypeObject Py_TextEncoding;

bool registerFuseText(PyObject* module);
}