#include "addresses.h"
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/RelativeStorageFormat.h>

using namespace fuse;

//##[ AddressFormat ]##############################################################################

static PyMethodDef PyAddressFormat_methods[] = {{NULL}};

static void PyAddressFormat_dealloc(PyAddressFormat* self)
{
    // delete self->format;
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyAddressFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._AddressFormat"};

static bool registerAddressFormat(PyObject* module)
{
    PyAddressFormatType.tp_basicsize = sizeof(PyAddressFormat);
    PyAddressFormatType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyAddressFormatType.tp_doc = "Describes the format of an address";
    PyAddressFormatType.tp_methods = PyAddressFormat_methods;
    PyAddressFormatType.tp_dealloc = (destructor)&PyAddressFormat_dealloc;
    if (PyType_Ready(&PyAddressFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyAddressFormatType);
    PyModule_AddObject(module, "_AddressFormat", (PyObject*)&PyAddressFormatType);
    return true;
}

//##[ AddressLayout ]##############################################################################

static PyMethodDef PyAddressLayout_methods[] = {{NULL}};

static void PyAddressLayout_dealloc(PyAddressLayout* self)
{
    delete self->layout;
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyAddressLayoutType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._AddressLayout"};

static bool registerAddressLayout(PyObject* module)
{
    PyAddressLayoutType.tp_basicsize = sizeof(PyAddressLayout);
    PyAddressLayoutType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyAddressLayoutType.tp_doc = "Describes the format of an address";
    PyAddressLayoutType.tp_methods = PyAddressLayout_methods;
    PyAddressLayoutType.tp_dealloc = (destructor)&PyAddressLayout_dealloc;
    if (PyType_Ready(&PyAddressLayoutType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyAddressLayoutType);
    PyModule_AddObject(module, "_AddressLayout", (PyObject*)&PyAddressLayoutType);
    return true;
}

//##[ RelativeAddressLayout ]######################################################################

static int PyRelativeAddressLayout_init(PyRelativeAddressLayout* self, PyObject*, PyObject*)
{
    self->base.layout = new RelativeStorageFormat;
    return 0;
}

static auto PyRelativeAddressLayout_set_layout(PyRelativeAddressLayout* self, PyObject* const* args,
                                               const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 3)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 3");
        return NULL;
    }

    auto const size = PyLong_AsUnsignedLongLong(args[0]);
    if (size == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const signedness = PyLong_AsUnsignedLongLong(args[1]);
    if (signedness == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const endianness = PyLong_AsUnsignedLongLong(args[2]);
    if (endianness == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    IntegerLayout layout;
    layout.sizeInBytes = size;
    layout.signedness = static_cast<Signedness>(signedness);
    layout.endianness = static_cast<Endianness>(endianness);
    static_cast<RelativeStorageFormat*>(self->base.layout)->setOffsetFormat(layout);

    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyRelativeAddressLayout_set_fixed_base_address(PyRelativeAddressLayout* self,
                                                           PyObject* pyAddress) -> PyObject*
{
    auto const address = PyLong_AsUnsignedLongLong(pyAddress);
    if (address == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto maybeAddress = fileOffsetFormat()->fromInteger(address);
    if (!maybeAddress)
    {
        PyErr_SetString(PyExc_RuntimeError, "could not convert into Address");
        return NULL;
    }
    static_cast<RelativeStorageFormat*>(self->base.layout)->setBaseAddress(*maybeAddress);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyRelativeAddressLayout_methods[] = {
    {"set_layout", (PyCFunction)PyRelativeAddressLayout_set_layout, METH_FASTCALL,
     "append an element format to the Record"},
    {"set_fixed_base_address", (PyCFunction)PyRelativeAddressLayout_set_fixed_base_address, METH_O,
     "sets the base of the RelativeAddressLayout to a fixed address"},
    {NULL}};

PyTypeObject PyRelativeAddressLayoutType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._RelativeAddressLayout"};

static bool registerRelativeAddressLayout(PyObject* module)
{
    PyRelativeAddressLayoutType.tp_new = PyType_GenericNew;
    PyRelativeAddressLayoutType.tp_base = &PyAddressLayoutType;
    PyRelativeAddressLayoutType.tp_basicsize = sizeof(PyRelativeAddressLayout);
    PyRelativeAddressLayoutType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyRelativeAddressLayoutType.tp_doc = "Describes layout of a relative address";
    PyRelativeAddressLayoutType.tp_methods = PyRelativeAddressLayout_methods;
    PyRelativeAddressLayoutType.tp_init = (initproc)&PyRelativeAddressLayout_init;
    if (PyType_Ready(&PyRelativeAddressLayoutType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyRelativeAddressLayoutType);
    PyModule_AddObject(module, "_RelativeAddressLayout", (PyObject*)&PyRelativeAddressLayoutType);
    return true;
}

//#################################################################################################

bool registerFuseAddresses(PyObject* module)
{
    if (!registerAddressFormat(module))
    {
        return false;
    }
    if (!registerAddressLayout(module))
    {
        return false;
    }
    if (!registerRelativeAddressLayout(module))
    {
        return false;
    }

    PyAddressFormat* fileOffset = PyObject_New(PyAddressFormat, &PyAddressFormatType);
    fileOffset->format = fileOffsetFormat();
    if (PyModule_AddObject(module, "_FileOffset", (PyObject*)fileOffset) < 0)
    {
        return false;
    }

    return true;
}
