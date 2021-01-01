#include "linking.h"
#include "addresses.h"
#include <fuse/linking/Backtracker.h>

using namespace fuse;

static int PyBacktrackingPacker_init(PyBacktrackingPacker* self, PyObject*, PyObject*)
{
    self->packer = new BacktrackingPacker{};
    return 0;
}

static void PyBacktrackingPacker_dealloc(PyBacktrackingPacker* self)
{
    delete self->packer;
    Py_TYPE(self)->tp_free(self);
}

static auto PyBacktrackingPacker_add_object(PyBacktrackingPacker* self, PyObject* pySize)
    -> PyObject*
{
    auto const size = PyLong_AsUnsignedLongLong(pySize);
    if (size == static_cast<decltype(size)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    std::string id = "obj_" + std::to_string(self->packer->objectCount());
    self->packer->addObject(std::make_unique<LinkObject>(id, static_cast<size_t>(size)));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyBacktrackingPacker_add_free_block(PyBacktrackingPacker* self, PyObject* const* args,
                                                const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 3)
    {
        PyErr_SetString(PyExc_TypeError, "expected 3 arguments");
        return NULL;
    }

    auto const offset = PyLong_AsUnsignedLongLong(args[0]);
    auto const size = PyLong_AsUnsignedLongLong(args[2]);
    auto const& address = reinterpret_cast<PyFuseAddress*>(args[1])->address;

    FreeBlock block{offset, address, size};
    self->packer->addFreeBlock(block);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyBacktrackingPacker_pack(PyBacktrackingPacker* self) -> PyObject*
{
    if (self->packer->pack())
    {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else
    {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static auto PyBacktrackingPacker_get_link_offset(PyBacktrackingPacker* self, PyObject* pyIndex)
    -> PyObject*
{
    auto const index = PyLong_AsUnsignedLongLong(pyIndex);
    if (index == static_cast<decltype(index)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (index >= self->packer->objectCount())
    {
        PyErr_SetString(PyExc_IndexError, "out of range");
        return NULL;
    }
    auto const& object = *self->packer->object(index);
    if (!object.hasAllocation())
    {
        PyErr_SetString(PyExc_ValueError, "object has no allocated address");
        return NULL;
    }
    return Py_BuildValue("K", static_cast<unsigned long long>(object.allocation().offset));
}

static auto PyBacktrackingPacker_get_link_address(PyBacktrackingPacker* self, PyObject* pyIndex)
    -> PyObject*
{
    auto const index = PyLong_AsUnsignedLongLong(pyIndex);
    if (index == static_cast<decltype(index)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (index >= self->packer->objectCount())
    {
        PyErr_SetString(PyExc_IndexError, "out of range");
        return NULL;
    }
    auto const& object = *self->packer->object(index);
    if (!object.hasAllocation())
    {
        PyErr_SetString(PyExc_ValueError, "object has no allocated address");
        return NULL;
    }

    PyFuseAddress* pyAddress = PyObject_New(PyFuseAddress, &PyFuseAddressType);
    new (&pyAddress->address) Address{object.allocation().address};
    return (PyObject*)pyAddress;
}

static auto PyBacktrackingPacker_set_log_file(PyBacktrackingPacker* self, PyObject* pyLogFile)
    -> PyObject*
{
    const char* szLogFile = PyUnicode_AsUTF8(pyLogFile);
    if (!szLogFile)
    {
        return NULL;
    }
    self->packer->setLogFile(szLogFile);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyBacktrackingPacker_methods[] = {
    {"add_object", (PyCFunction)PyBacktrackingPacker_add_object, METH_O, "add an object to pack"},
    {"add_free_block", (PyCFunction)PyBacktrackingPacker_add_free_block, METH_FASTCALL,
     "add a free block to pack into"},
    {"pack", (PyCFunction)PyBacktrackingPacker_pack, METH_NOARGS,
     "pack the objects into the free blocks"},
    {"get_link_offset", (PyCFunction)PyBacktrackingPacker_get_link_offset, METH_O,
     "get the offset allocated to the given object"},
    {"get_link_address", (PyCFunction)PyBacktrackingPacker_get_link_address, METH_O,
     "get the address allocated to the given object"},
    {"set_log_file", (PyCFunction)PyBacktrackingPacker_set_log_file, METH_O,
     "turn on logging and set the destination file"},
    {NULL}};

PyTypeObject PyBacktrackingPackerType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._BacktrackingPacker"};

static bool registerBacktrackingPacker(PyObject* module)
{
    PyBacktrackingPackerType.tp_new = PyType_GenericNew;
    PyBacktrackingPackerType.tp_basicsize = sizeof(PyBacktrackingPacker);
    PyBacktrackingPackerType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyBacktrackingPackerType.tp_doc = "Manages the context during decoding DataFormats";
    PyBacktrackingPackerType.tp_methods = PyBacktrackingPacker_methods;
    PyBacktrackingPackerType.tp_init = (initproc)&PyBacktrackingPacker_init;
    PyBacktrackingPackerType.tp_dealloc = (destructor)&PyBacktrackingPacker_dealloc;

    if (PyType_Ready(&PyBacktrackingPackerType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyBacktrackingPackerType);
    PyModule_AddObject(module, "_BacktrackingPacker", (PyObject*)&PyBacktrackingPackerType);
    return true;
}

bool registerLinkingTypes(PyObject* module)
{
    if (!registerBacktrackingPacker(module))
    {
        return false;
    }
    return true;
}
