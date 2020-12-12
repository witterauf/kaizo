#include "addresses.h"
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/MipsEmbeddedLayout.h>
#include <fuse/addresses/RegionAddressMap.h>
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

static auto PyRelativeAddressLayout_set_null_pointer(PyRelativeAddressLayout* self,
                                                     PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 2");
        return NULL;
    }

    auto const address = PyLong_AsUnsignedLongLong(args[0]);
    if (address == static_cast<decltype(address)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const offset = PyLong_AsLongLong(args[1]);
    if (offset == static_cast<decltype(offset)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto maybeAddress = fileOffsetFormat()->fromInteger(address);
    if (!maybeAddress)
    {
        PyErr_SetString(PyExc_RuntimeError, "could not convert into Address");
        return NULL;
    }

    static_cast<RelativeStorageFormat*>(self->base.layout)->setNullPointer(*maybeAddress, offset);

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
    {"set_null_pointer", (PyCFunction)PyRelativeAddressLayout_set_null_pointer, METH_FASTCALL,
     "sets the offsets which is considered the null pointer"},
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

//##[ PyMipsEmbeddedLayout ]#######################################################################

static int PyMipsEmbeddedLayout_init(PyMipsEmbeddedLayout* self, PyObject*, PyObject*)
{
    self->base.layout = new MipsEmbeddedLayout;
    return 0;
}

static auto PyMipsEmbeddedLayout_set_offsets(PyMipsEmbeddedLayout* self, PyObject* const* args,
                                             const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 2");
        return NULL;
    }

    auto const hi16 = PyLong_AsLongLong(args[0]);
    if (hi16 == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const lo16 = PyLong_AsLongLong(args[1]);
    if (lo16 == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    static_cast<MipsEmbeddedLayout*>(self->base.layout)->setOffsets(hi16, lo16);

    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyMipsEmbeddedLayout_set_base_address(PyMipsEmbeddedLayout* self, PyObject* pyAddress)
    -> PyObject*
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
    static_cast<MipsEmbeddedLayout*>(self->base.layout)->setBaseAddress(*maybeAddress);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyMipsEmbeddedLayout_methods[] = {
    {"set_offsets", (PyCFunction)PyMipsEmbeddedLayout_set_offsets, METH_FASTCALL,
     "set the offsets of the MSB and LSB parts of the pointer"},
    {"set_base_address", (PyCFunction)PyMipsEmbeddedLayout_set_base_address, METH_O,
     "set the base address of the pointer"},
    {NULL}};

PyTypeObject PyMipsEmbeddedLayoutType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._MipsEmbeddedLayout"};

static bool registerMipsEmbeddedLayout(PyObject* module)
{
    PyMipsEmbeddedLayoutType.tp_new = PyType_GenericNew;
    PyMipsEmbeddedLayoutType.tp_base = &PyAddressLayoutType;
    PyMipsEmbeddedLayoutType.tp_basicsize = sizeof(PyMipsEmbeddedLayout);
    PyMipsEmbeddedLayoutType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyMipsEmbeddedLayoutType.tp_doc = "Describes layout of a relative address";
    PyMipsEmbeddedLayoutType.tp_methods = PyMipsEmbeddedLayout_methods;
    PyMipsEmbeddedLayoutType.tp_init = (initproc)&PyMipsEmbeddedLayout_init;
    if (PyType_Ready(&PyMipsEmbeddedLayoutType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyMipsEmbeddedLayoutType);
    PyModule_AddObject(module, "_MipsEmbeddedLayout", (PyObject*)&PyMipsEmbeddedLayoutType);
    return true;
}

//##[ AddressMap ]#################################################################################

static PyMethodDef PyAddressMap_methods[] = {{NULL}};

static void PyAddressMap_dealloc(PyAddressMap* self)
{
    delete self->map;
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyAddressMapType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._AddressMap"};

static bool registerAddressMap(PyObject* module)
{
    PyAddressMapType.tp_basicsize = sizeof(PyAddressMap);
    PyAddressMapType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyAddressMapType.tp_doc = "Describes a map from one AddressFormat to another";
    PyAddressMapType.tp_methods = PyAddressMap_methods;
    PyAddressMapType.tp_dealloc = (destructor)&PyAddressMap_dealloc;
    if (PyType_Ready(&PyAddressMapType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyAddressMapType);
    PyModule_AddObject(module, "_AddressMap", (PyObject*)&PyAddressMapType);
    return true;
}

//##[ RegionedAddressMap ]######################################################################

static int PyRegionedAddressMap_init(PyRegionedAddressMap* self, PyObject* args, PyObject*)
{
    PyObject* pySource{nullptr};
    PyObject* pyTarget{nullptr};
    if (PyArg_ParseTuple(args, "OO", &pySource, &pyTarget) < 0)
    {
        return -1;
    }

    if (!PyObject_IsInstance(pySource, (PyObject*)&PyAddressFormatType) ||
        !PyObject_IsInstance(pyTarget, (PyObject*)&PyAddressFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "expected two _AddressFormats");
        return -1;
    }

    auto const* source = reinterpret_cast<PyAddressFormat*>(pySource)->format;
    auto const* target = reinterpret_cast<PyAddressFormat*>(pyTarget)->format;

    self->base.map = new RegionAddressMap{source, target};
    return 0;
}

static auto PyRegionedAddressMap_add_region(PyRegionedAddressMap* self, PyObject* const* args,
                                            const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 3)
    {
        PyErr_SetString(PyExc_RuntimeError, "wrong number of arguments; expected 3");
        return NULL;
    }

    std::optional<Address> maybeSource;
    if (PyLong_Check(args[0]))
    {
        auto const address = PyLong_AsUnsignedLongLong(args[0]);
        if (auto const maybeAddress = self->base.map->sourceFormat().fromInteger(address))
        {
            maybeSource = maybeAddress;
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "could not convert address");
            return NULL;
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "expected an integer");
        return NULL;
    }

    std::optional<Address> maybeTarget;
    if (PyLong_Check(args[1]))
    {
        auto const address = PyLong_AsUnsignedLongLong(args[1]);
        if (auto const maybeAddress = self->base.map->sourceFormat().fromInteger(address))
        {
            maybeTarget = maybeAddress;
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "could not convert address");
            return NULL;
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "expected an integer");
        return NULL;
    }

    auto const size = PyLong_AsUnsignedLongLong(args[2]);
    if (size == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    static_cast<RegionAddressMap*>(self->base.map)->map(*maybeSource, *maybeTarget, size);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyRegionedAddressMap_methods[] = {
    {"add_region", (PyCFunction)PyRegionedAddressMap_add_region, METH_FASTCALL,
     "add a region to the RegionedAddressMap"},
    {NULL}};

PyTypeObject PyRegionedAddressMapType = {
    PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._RegionedAddressMap"};

static bool registerRegionedAddressMap(PyObject* module)
{
    PyRegionedAddressMapType.tp_new = PyType_GenericNew;
    PyRegionedAddressMapType.tp_base = &PyAddressMapType;
    PyRegionedAddressMapType.tp_basicsize = sizeof(PyRegionedAddressMap);
    PyRegionedAddressMapType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyRegionedAddressMapType.tp_doc = "Describes an AddressMap partitioned into regions";
    PyRegionedAddressMapType.tp_methods = PyRegionedAddressMap_methods;
    PyRegionedAddressMapType.tp_init = (initproc)&PyRegionedAddressMap_init;
    if (PyType_Ready(&PyRegionedAddressMapType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyRegionedAddressMapType);
    PyModule_AddObject(module, "_RegionedAddressMap", (PyObject*)&PyRegionedAddressMapType);
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
    if (!registerMipsEmbeddedLayout(module))
    {
        return false;
    }
    if (!registerAddressMap(module))
    {
        return false;
    }
    if (!registerRegionedAddressMap(module))
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
