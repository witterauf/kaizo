#include "binary.h"
#include "addresses.h"
#include "objects.h"
#include <fuse/BinaryView.h>

using namespace fuse;
using namespace fuse::binary;

//##[ DataReader ]#################################################################################

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

//##[ DataWriter ]#################################################################################

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

//##[ BinaryPatch ]################################################################################

auto PyFuseBinaryPatch_New(const uint64_t data, const uint64_t mask, const size_t size,
                           const ptrdiff_t offset) -> PyObject*
{
    auto* pyPatch = PyObject_New(PyFuseBinaryPatch, &PyFuseBinaryPatchType);
    new (&pyPatch->patch) BinaryPatch{data, mask, size, offset};
    return (PyObject*)pyPatch;
}

auto PyFuseBinaryPatch_New(const BinaryPatch& patch) -> PyObject*
{
    auto* pyPatch = PyObject_New(PyFuseBinaryPatch, &PyFuseBinaryPatchType);
    new (&pyPatch->patch) BinaryPatch{patch};
    return (PyObject*)pyPatch;
}

/*
static int PyFuseBinaryPatch_init(PyFuseBinaryPatch* self, PyObject* args, PyObject*)
{
    const uint8_t* byData;
    int dataSize{-1};
    const uint8_t* byMask;
    int maskSize{-1};
    long long offset;
    if (PyArg_ParseTuple(args, "y#y#K", &byData, &dataSize, byMask, &maskSize, &offset) < 0)
    {
        return -1;
    }

    new (&self->patch) BinaryPatch{data, mask, size, offset};
    return 0;
}
*/

static void PyFuseBinaryPatch_dealloc(PyFuseBinaryPatch* self)
{
    self->patch.~BinaryPatch();
    Py_TYPE(self)->tp_free(self);
}

static auto PyFuseBinaryPatch_apply(PyFuseBinaryPatch* self, PyObject* const* args,
                                    const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_TypeError, "expected 2 arguments (target, offset)");
        return NULL;
    }

    auto const offset = PyLong_AsUnsignedLongLong(args[1]);
    if (offset == static_cast<decltype(offset)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    Py_buffer buffer;
    if (PyObject_GetBuffer(args[0], &buffer, PyBUF_C_CONTIGUOUS | PyBUF_SIMPLE | PyBUF_WRITEABLE) <
        0)
    {
        return NULL;
    }

    MutableBinaryView binary{reinterpret_cast<uint8_t*>(buffer.buf),
                             static_cast<size_t>(buffer.len)};

    auto const effectiveOffset = offset + self->patch.relativeOffset();
    if (effectiveOffset + self->patch.size() > binary.size())
    {
        PyErr_SetString(PyExc_IndexError,
                        ("patch (offsets " + std::to_string(effectiveOffset) + " - " +
                         std::to_string(effectiveOffset + self->patch.size()) +
                         ") spills out of binary (size " + std::to_string(binary.size()) + ")")
                            .c_str());
        PyBuffer_Release(&buffer);
        return NULL;
    }

    self->patch.apply(binary, offset);

    PyBuffer_Release(&buffer);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyFuseBinaryPatch_effective_offset(PyFuseBinaryPatch* self, PyObject* pyOffset)
    -> PyObject*
{
    auto const offset = PyLong_AsUnsignedLongLong(pyOffset);
    if (offset == static_cast<decltype(offset)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    return Py_BuildValue("K",
                         static_cast<unsigned long long>(offset + self->patch.relativeOffset()));
}

static auto PyFuseBinaryPatch_is_partial(PyFuseBinaryPatch* self, void*) -> PyObject*
{
    if (self->patch.usesOnlyFullBytes())
    {
        Py_INCREF(Py_False);
        return Py_False;
    }
    else
    {
        Py_INCREF(Py_True);
        return Py_True;
    }
}

static auto PyFuseBinaryPatch_len(PyFuseBinaryPatch* self) -> Py_ssize_t
{
    return static_cast<Py_ssize_t>(self->patch.size());
}

static PyMethodDef PyFuseBinaryPatch_methods[] = {
    {"apply", (PyCFunction)PyFuseBinaryPatch_apply, METH_FASTCALL,
     "applies the BinaryPatch to an object supporting the buffer protocols"},
    {"effective_offset", (PyCFunction)PyFuseBinaryPatch_effective_offset, METH_O,
     "compute the effective offset of the patch"},
    {NULL}};

PyGetSetDef PyFuseBinaryPatch_getsets[] = {
    {"is_partial", (getter)PyFuseBinaryPatch_is_partial, NULL,
     "get whether the BinaryPatch modifies partial bytes", NULL},
    {NULL}};

static PySequenceMethods PyFuseBinaryPatch_seqmethods;

PyTypeObject PyFuseBinaryPatchType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy.BinaryPatch"};

static bool registerBinaryPatch(PyObject* module)
{
    memset(&PyFuseBinaryPatch_seqmethods, 0, sizeof(PySequenceMethods));
    PyFuseBinaryPatch_seqmethods.sq_length = (lenfunc)&PyFuseBinaryPatch_len;

    PyFuseBinaryPatchType.tp_new = PyType_GenericNew;
    PyFuseBinaryPatchType.tp_basicsize = sizeof(PyFuseBinaryPatch);
    PyFuseBinaryPatchType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyFuseBinaryPatchType.tp_doc = "Represents a lightweight patch to binary data";
    PyFuseBinaryPatchType.tp_methods = PyFuseBinaryPatch_methods;
    PyFuseBinaryPatchType.tp_as_sequence = &PyFuseBinaryPatch_seqmethods;
    PyFuseBinaryPatchType.tp_getset = PyFuseBinaryPatch_getsets;
    // PyFuseBinaryPatchType.tp_init = (initproc)&PyFuseBinaryPatch_init;
    PyFuseBinaryPatchType.tp_dealloc = (destructor)&PyFuseBinaryPatch_dealloc;

    if (PyType_Ready(&PyFuseBinaryPatchType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyFuseBinaryPatchType);
    PyModule_AddObject(module, "BinaryPatch", (PyObject*)&PyFuseBinaryPatchType);
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
    if (!registerBinaryPatch(module))
    {
        return false;
    }
    return true;
}