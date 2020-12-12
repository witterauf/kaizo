#pragma once

#include <fuse/text/TextEncoding.h>

#ifdef _WIN32
#ifdef FUSE_WIN_EXPORT
#define FUSE_EXPORTED __declspec(dllexport)
#else
#define FUSE_EXPORTED __declspec(dllimport)
#endif
#else
#define FUSE_EXPORTED
#endif

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyTextEncoding
{
    PyObject_HEAD;
    std::shared_ptr<fuse::text::TextEncoding> encoding;
};

extern FUSE_EXPORTED PyTypeObject Py_TextEncoding;

bool registerFuseText(PyObject* module);
}