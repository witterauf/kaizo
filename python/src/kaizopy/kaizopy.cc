#include "kaizopy.h"
#include <optional>

using namespace kaizo;

//##[ VirtualFileSystem ]##########################################################################

static auto PyVirtualFileSystem_file_count_get(PyVirtualFileSystem* pyVfs, void*)
    -> PyObject*
{
    return PyLong_FromSize_t(pyVfs->vfs->fileCount());
}

static auto toFileIndex(PyVirtualFileSystem* pyVfs, PyObject* arg) -> std::optional<size_t>
{
    if (!PyLong_Check(arg))
    {
        PyErr_SetString(PyExc_TypeError, "expected a positive integer or zero");
        return {};
    }

    auto const index = PyLong_AsSize_t(arg);
    if (index == static_cast<size_t>(-1) && PyErr_Occurred())
    {
        PyErr_SetString(PyExc_OverflowError, "expected a positive integer or zero");
        return {};
    }

    if (index >= pyVfs->vfs->fileCount())
    {
        PyErr_SetString(PyExc_ValueError, "index exceeds number of files");
        return {};
    }

    return index;
}

static auto PyVirtualFileSystem_is_folder(PyVirtualFileSystem* pyVfs, PyObject* arg) -> PyObject*
{
    if (auto maybeIndex = toFileIndex(pyVfs, arg))
    {
        if (pyVfs->vfs->isFolder(*maybeIndex))
        {
            Py_RETURN_TRUE;
        }
        else
        {
            Py_RETURN_FALSE;
        }
    }
    else
    {
        return NULL;
    }
}

static auto PyVirtualFileSystem_file_size(PyVirtualFileSystem* pyVfs, PyObject* arg) -> PyObject*
{
    if (auto maybeIndex = toFileIndex(pyVfs, arg))
    {
        return PyLong_FromSize_t(pyVfs->vfs->fileSize(*maybeIndex));
    }
    else
    {
        return NULL;
    }
}

static auto PyVirtualFileSystem_file_name(PyVirtualFileSystem* pyVfs, PyObject* arg) -> PyObject*
{
    if (auto maybeIndex = toFileIndex(pyVfs, arg))
    {
        auto const fileName = pyVfs->vfs->fileName(*maybeIndex);
        return PyUnicode_FromString(fileName.c_str());
    }
    else
    {
        return NULL;
    }
}

static auto PyVirtualFileSystem_file_index(PyVirtualFileSystem* pyVfs, PyObject* arg) -> PyObject*
{
    const char* name = PyUnicode_AsUTF8(arg);
    if (!name)
    {
        PyErr_SetString(PyExc_TypeError, "expected a string");
        return NULL;
    }

    if (auto const maybeIndex = pyVfs->vfs->fileIndex(name))
    {
        return PyLong_FromUnsignedLongLong(*maybeIndex);
    }
    else
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

static auto PyVirtualFileSystem_open_folder(PyVirtualFileSystem* self, PyObject* arg) -> PyObject*
{
    if (auto maybeIndex = toFileIndex(self, arg))
    {
        auto* pyVfs = PyObject_New(PyVirtualFileSystem, &PyVirtualFileSystemType);
        try
        {
            pyVfs->vfs = self->vfs->openFolder(*maybeIndex).release();
        }
        catch (std::exception& e)
        {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
        return (PyObject*)pyVfs;
    }
    else
    {
        return NULL;
    }
}

static auto PyVirtualFileSystem_open_file(PyVirtualFileSystem* self, PyObject* arg) -> PyObject*
{
    if (auto maybeIndex = toFileIndex(self, arg))
    {
        try
        {
            auto binary = self->vfs->openFile(*maybeIndex);
            auto* bytes = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(binary.data()),
                                                    binary.size());
            return bytes;
        }
        catch (std::exception& e)
        {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

static void PyVirtualFileSystem_dealloc(PyVirtualFileSystem* self)
{
    if (self->vfs)
    {
        delete self->vfs;
    }
    Py_TYPE(self)->tp_free(self);
}

static PyGetSetDef PyVirtualFileSystem_properties[] = {
    {"file_count", (getter)PyVirtualFileSystem_file_count_get, NULL,
     "number of files on this level", NULL},
    {NULL}};

static PyMethodDef PyVirtualFileSystem_methods[] = {
    {"is_folder", (PyCFunction)PyVirtualFileSystem_is_folder, METH_O,
     "return whether the file with the given index is a folder"},
    {"file_size", (PyCFunction)PyVirtualFileSystem_file_size, METH_O,
     "return the size of the file with the given index"},
    {"file_name", (PyCFunction)PyVirtualFileSystem_file_name, METH_O,
     "return the name of the file with the given index"},
    {"file_index", (PyCFunction)PyVirtualFileSystem_file_index, METH_O,
     "return the index of the file with the given name"},
    {"open_folder", (PyCFunction)PyVirtualFileSystem_open_folder, METH_O,
     "return a new virtual file system representing the folder with the given index"},
    {"open_file", (PyCFunction)PyVirtualFileSystem_open_file, METH_O,
     "return a bytes object with the content of the file with the given index"},
    {NULL}};

PyTypeObject PyVirtualFileSystemType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._VirtualFileSystem"};

static bool registerVirtualFileSystem(PyObject* module)
{
    PyVirtualFileSystemType.tp_basicsize = sizeof(PyVirtualFileSystem);
    PyVirtualFileSystemType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyVirtualFileSystemType.tp_doc = "Represents a virtual file system within a buffer or file";
    PyVirtualFileSystemType.tp_methods = PyVirtualFileSystem_methods;
    PyVirtualFileSystemType.tp_getset = PyVirtualFileSystem_properties;
    PyVirtualFileSystemType.tp_dealloc = (destructor)PyVirtualFileSystem_dealloc;
    if (PyType_Ready(&PyVirtualFileSystemType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyVirtualFileSystemType);
    PyModule_AddObject(module, "_VirtualFileSystem", (PyObject*)&PyVirtualFileSystemType);
    return true;
}

//##[ FileTypeDescriptor ]#########################################################################

static PyMethodDef PyFileTypeDescriptor_methods[] = {{NULL}};

PyTypeObject PyFileTypeDescriptorType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy.FileTypeDescriptor"};

static void PyFileTypeDescriptor_dealloc(PyFileTypeDescriptor* self)
{
    self->descriptor.reset();
    Py_TYPE(self)->tp_free(self);
}

static bool registerFileTypeDescriptor(PyObject* module)
{
    PyFileTypeDescriptorType.tp_basicsize = sizeof(PyFileTypeDescriptor);
    PyFileTypeDescriptorType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyFileTypeDescriptorType.tp_doc = "Describes a file type within a virtual file system";
    PyFileTypeDescriptorType.tp_methods = PyFileTypeDescriptor_methods;
    PyFileTypeDescriptorType.tp_dealloc = (destructor)PyFileTypeDescriptor_dealloc;
    if (PyType_Ready(&PyFileTypeDescriptorType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyFileTypeDescriptorType);
    if (PyModule_AddObject(module, "FileTypeDescriptor", (PyObject*)&PyFileTypeDescriptorType) < 0)
    {
        Py_DECREF(&PyFileTypeDescriptorType);
        return false;
    }
    return true;
}

auto PyFileTypeDescriptor_New(std::shared_ptr<kaizo::FileTypeDescriptor> descriptor) -> PyObject*
{
    auto* obj = PyObject_New(PyFileTypeDescriptor, &PyFileTypeDescriptorType);
    // Emplacement new necessary. Otherwise, the shared_ptr would try to destruct the invalid value
    // that has been initialized by the Python runtime.
    new (&obj->descriptor) std::shared_ptr<kaizo::FileTypeDescriptor>{descriptor};
    return (PyObject*)obj;
}

//#################################################################################################

bool registerVirtualFileSystemTypes(PyObject* module)
{
    if (!registerVirtualFileSystem(module))
    {
        return false;
    }
    if (!registerFileTypeDescriptor(module))
    {
        return false;
    }

    PyObject* pyRegularFile = PyFileTypeDescriptor_New(std::make_shared<RegularFile>());
    if (PyModule_AddObject(module, "RegularFile", pyRegularFile) < 0)
    {
        Py_DECREF(pyRegularFile);
        return false;
    }

    return true;
}

static PyMethodDef kaizopy_functions[] = {
    {NULL},
};

static PyModuleDef kaizopymodule = {PyModuleDef_HEAD_INIT,
                                    "_kaizopy",
                                    "Utility classes and functions for the Tales of series",
                                    -1,
                                    kaizopy_functions,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL};

PyMODINIT_FUNC PyInit__kaizopy(void)
{
    PyObject* m = PyModule_Create(&kaizopymodule);
    if (m == NULL)
    {
        return NULL;
    }

    if (!registerVirtualFileSystemTypes(m))
    {
        return NULL;
    }

    return m;
}
