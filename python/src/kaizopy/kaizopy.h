#pragma once

#include <kaizo/vfs/VirtualFileSystem.h>

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

extern EXPORTED PyTypeObject PyVirtualFileSystemType;
extern EXPORTED PyTypeObject PyFileTypeDescriptorType;

bool registerVirtualFileSystem(PyObject* module);

auto EXPORTED PyFileTypeDescriptor_New(std::shared_ptr<kaizo::FileTypeDescriptor>) -> PyObject*;
}