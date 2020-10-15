#include "text.h"
#include <fuse/text/AsciiEncoding.h>
#include <iostream>

using namespace fuse;

//##[ VirtualFileSystem ]##########################################################################

static auto PyTextEncoding_supports_encoding(PyTextEncoding* self, void*) -> PyObject*
{
    if (self->encoding->canEncode())
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

static auto PyTextEncoding_supports_decoding(PyTextEncoding* self, void*) -> PyObject*
{
    if (self->encoding->canDecode())
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

static auto PyTextEncoding_encode(PyTextEncoding* self, PyObject* arg) -> PyObject*
{
    if (!self->encoding->canEncode())
    {
        PyErr_SetString(PyExc_RuntimeError, "This text encoding does not support encoding");
    }

    const char* text = PyUnicode_AsUTF8(arg);
    if (!text)
    {
        PyErr_SetString(PyExc_TypeError, "expected a string");
        return NULL;
    }

    auto binary = self->encoding->encode(text);
    auto* bytes =
        PyBytes_FromStringAndSize(reinterpret_cast<const char*>(binary.data()), binary.size());
    return bytes;
}

static auto PyTextEncoding_decode(PyTextEncoding* self, PyObject* args, PyObject*) -> PyObject*
{
    Py_buffer buffer;
    unsigned long long offset{0};
    if (!PyArg_ParseTuple(args, "y*K", &buffer, &offset))
    {
        return NULL;
    }

    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.buf);
    auto const size = buffer.len;
    try
    {
        auto const [newOffset, string] =
            self->encoding->decode(BinaryView{data, static_cast<size_t>(size)}, offset);
        PyBuffer_Release(&buffer);
        auto tuple = Py_BuildValue("(Ks#)", newOffset, string.c_str(), (Py_ssize_t)string.length());
        return tuple;
    }
    catch (std::exception& e)
    {
        PyBuffer_Release(&buffer);
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }
}

static void PyTextEncoding_dealloc(PyTextEncoding* self)
{
    if (self->encoding)
    {
        delete self->encoding;
    }
    Py_TYPE(self)->tp_free(self);
}

static PyGetSetDef PyVirtualFileSystem_properties[] = {
    {"supports_encoding", (getter)PyTextEncoding_supports_encoding, NULL,
     "whether this text encoding supports encoding from a string into bytes", NULL},
    {"supports_decoding", (getter)PyTextEncoding_supports_encoding, NULL,
     "whether this text encoding supports decoding a string from bytes", NULL},
    {NULL}};

static PyMethodDef PyVirtualFileSystem_methods[] = {
    {"encode", (PyCFunction)PyTextEncoding_encode, METH_O,
     "encode the given string into a sequence of bytes"},
    {"decode", (PyCFunction)PyTextEncoding_decode, METH_VARARGS,
     "decode the given sequence of bytes, starting at offset, into a string"},
    {NULL}};

PyTypeObject Py_TextEncoding = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._TextEncoding"};

bool registerTextEncoding(PyObject* module)
{
    Py_TextEncoding.tp_basicsize = sizeof(PyTextEncoding);
    Py_TextEncoding.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    Py_TextEncoding.tp_doc = "Represents a TextEncoding";
    Py_TextEncoding.tp_methods = PyVirtualFileSystem_methods;
    Py_TextEncoding.tp_getset = PyVirtualFileSystem_properties;
    Py_TextEncoding.tp_dealloc = (destructor)PyTextEncoding_dealloc;
    if (PyType_Ready(&Py_TextEncoding) < 0)
    {
        return false;
    }
    Py_INCREF(&Py_TextEncoding);
    PyModule_AddObject(module, "_TextEncoding", (PyObject*)&Py_TextEncoding);

    auto* ascii = PyObject_New(PyTextEncoding, &Py_TextEncoding);
    ascii->encoding = new fuse::text::AsciiEncoding;
    PyModule_AddObject(module, "_AsciiEncoding", (PyObject*)ascii);

    return true;
}

bool registerFuseText(PyObject* module)
{
    if (!registerTextEncoding(module))
    {
        return false;
    }
    return true;
}
