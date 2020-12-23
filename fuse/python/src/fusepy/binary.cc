#include "binary.h"
#include "addresses.h"
#include "objects.h"
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

static void PyDataReader_dealloc(PyDataReader* self)
{
    self->reader.~DataReader();
    Py_TYPE(self)->tp_free(self);
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
    PyDataReaderType.tp_dealloc = (destructor)&PyDataReader_dealloc;

    if (PyType_Ready(&PyDataReaderType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataReaderType);
    PyModule_AddObject(module, "_DataReader", (PyObject*)&PyDataReaderType);
    return true;
}

//##[ DataWriter ]######################################################################

static int PyDataWriter_init(PyDataWriter* self, PyObject*, PyObject*)
{
    new (&self->writer) DataWriter{};
    return 0;
}

static void PyDataWriter_dealloc(PyDataWriter* self)
{
    self->writer.~DataWriter();
    Py_TYPE(self)->tp_free(self);
}

static auto PyDataWriter_assemble(PyDataWriter* self) -> PyObject*
{
    auto objects = self->writer.assemble();
    return toPython(objects);
}

static PyMethodDef PyDataWriter_methods[] = {{"assemble", (PyCFunction)PyDataWriter_assemble,
                                              METH_NOARGS,
                                              "assembles the encoded data into objects"},
                                             {NULL}};

PyTypeObject PyDataWriterType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._DataWriter"};

static bool registerDataWriter(PyObject* module)
{
    PyDataWriterType.tp_new = PyType_GenericNew;
    PyDataWriterType.tp_basicsize = sizeof(PyDataWriter);
    PyDataWriterType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyDataWriterType.tp_doc = "Manages the context during encoding DataFormats";
    PyDataWriterType.tp_methods = PyDataWriter_methods;
    PyDataWriterType.tp_init = (initproc)&PyDataWriter_init;
    PyDataWriterType.tp_dealloc = (destructor)&PyDataWriter_dealloc;

    if (PyType_Ready(&PyDataWriterType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataWriterType);
    PyModule_AddObject(module, "_DataWriter", (PyObject*)&PyDataWriterType);
    return true;
}

//#################################################################################################

bool registerFuseBinary(PyObject* module)
{
    if (!registerDataReader(module))
    {
        return false;
    }
    if (!registerDataWriter(module))
    {
        return false;
    }
    return true;
}