#include "binary.h"
#include "addresses.h"
#include <fuse/BinaryView.h>

using namespace fuse;
using namespace fuse::binary;

//##[ DataReader ]######################################################################

static int PyDataReader_init(PyDataReader* self, PyObject* args, PyObject*)
{
    Py_buffer pyBuffer;
    if (!PyArg_ParseTuple(args, "y*", &pyBuffer))
    {
        return -1;
    }

    const uint8_t* data = reinterpret_cast<const uint8_t*>(pyBuffer.buf);
    auto const size = pyBuffer.len;
    new (&self->reader) DataReader{BinaryView{data, static_cast<size_t>(size)}};
    return 0;
}

static auto PyDataReader_set_offset(PyDataReader* self, PyObject* pyOffset) -> PyObject*
{
    auto const offset = PyLong_AsUnsignedLongLong(pyOffset);
    if (offset == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->reader.setOffset(offset);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataReader_set_address_map(PyDataReader* self, PyObject* pyAddressMap) -> PyObject*
{
    if (!PyObject_IsInstance(pyAddressMap, (PyObject*)&PyAddressMapType))
    {
        PyErr_SetString(PyExc_TypeError, "expected an _AddressMap");
        return NULL;
    }

    auto addressMap = reinterpret_cast<PyAddressMap*>(pyAddressMap)->map->copy();
    self->reader.setAddressMap(std::move(addressMap));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyDataReader_methods[] = {
    {"set_offset", (PyCFunction)PyDataReader_set_offset, METH_O,
     "set the offset from which to decode DataFormats"},
    {"set_address_map", (PyCFunction)PyDataReader_set_address_map, METH_O,
     "set the AddressMap with which PointerFormat addresses are translated to offsets"},
    {NULL}};

PyTypeObject PyDataReaderType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._DataReader"};

static bool registerDataReader(PyObject* module)
{
    PyDataReaderType.tp_new = PyType_GenericNew;
    PyDataReaderType.tp_basicsize = sizeof(PyDataReader);
    PyDataReaderType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyDataReaderType.tp_doc = "Manages the context during decoding DataFormats";
    PyDataReaderType.tp_methods = PyDataReader_methods;
    PyDataReaderType.tp_init = (initproc)&PyDataReader_init;
    if (PyType_Ready(&PyDataReaderType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataReaderType);
    PyModule_AddObject(module, "_DataReader", (PyObject*)&PyDataReaderType);
    return true;
}

//#################################################################################################

bool registerFuseBinary(PyObject* module)
{
    if (!registerDataReader(module))
    {
        return false;
    }
    return true;
}