#pragma once

#include <kaizo/vfs/VirtualFileSystem.h>

#ifdef _WIN32
#ifdef KAIZO_WIN_EXPORT
#define KAIZO_EXPORTED __declspec(dllexport)
#else
#define KAIZO_EXPORTED __declspec(dllimport)
#endif
#else
#define EXPORTED
#endif

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyVirtualFileSystem
{
    PyObject_HEAD;
    kaizo::VirtualFileSystem* vfs;
};

struct PyFileTypeDescriptor
{
    PyObject_HEAD;
    std::shared_ptr<kaizo::FileTypeDescriptor> descriptor;
};

extern KAIZO_EXPORTED PyTypeObject PyVirtualFileSystemType;
extern KAIZO_EXPORTED PyTypeObject PyFileTypeDescriptorType;

bool registerVirtualFileSystem(PyObject* module);

auto KAIZO_EXPORTED PyFileTypeDescriptor_New(std::shared_ptr<kaizo::FileTypeDescriptor>) -> PyObject*;
}