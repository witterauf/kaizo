#include "dataformat.h"
#include "addresses.h"
#include "binary.h"
#include "data.h"
#include "text.h"
#include <iostream>
#include <map>

using namespace fuse::binary;

//##[ DataFormat ]#################################################################################

static auto PyDataFormat_set_skip_before(PyDataFormat* self, PyObject* pySkip) -> PyObject*
{
    auto const skip = PyLong_AsUnsignedLongLong(pySkip);
    if (skip == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->format->setSkipBefore(skip);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataFormat_set_skip_after(PyDataFormat* self, PyObject* pySkip) -> PyObject*
{
    auto const skip = PyLong_AsUnsignedLongLong(pySkip);
    if (skip == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->format->setSkipAfter(skip);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataFormat_set_alignment(PyDataFormat* self, PyObject* pyAlignment) -> PyObject*
{
    auto const alignment = PyLong_AsUnsignedLongLong(pyAlignment);
    if (alignment == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->format->setAlignment(alignment);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataFormat_set_fixed_offset(PyDataFormat* self, PyObject* pyOffset) -> PyObject*
{
    auto const offset = PyLong_AsUnsignedLongLong(pyOffset);
    if (offset == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->format->setFixedOffset(offset);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataFormat_set_tag(PyDataFormat* self, PyObject* pyTag) -> PyObject*
{
    const char* tag = PyUnicode_AsUTF8(pyTag);
    if (!tag)
    {
        return NULL;
    }
    if (tag[0] == '\0')
    {
        PyErr_SetString(PyExc_ValueError, "expected a non-empty string");
        return NULL;
    }
    self->format->setTag(tag);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyDataFormat_decode(PyDataFormat* self, PyObject* pyReader) -> PyObject*
{
    if (!PyObject_IsInstance(pyReader, (PyObject*)&PyDataReaderType))
    {
        PyErr_SetString(PyExc_TypeError, "expected a _DataReader");
        return NULL;
    }

    auto& reader = reinterpret_cast<PyDataReader*>(pyReader)->reader;

    std::unique_ptr<Data> data;
    try
    {
        data = self->format->decode(reader);
    }
    catch (std::exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }

    if (!data)
    {
        PyErr_SetString(PyExc_RuntimeError, "could not decode DataFormat");
        return NULL;
    }

    auto* pyData = toNativePython(*data);
    if (!pyData)
    {
        return NULL;
    }

    return pyData;
}

static PyMethodDef PyDataFormat_methods[] = {
    {"set_skip_before", (PyCFunction)PyDataFormat_set_skip_before, METH_O,
     "set the number of bytes to skip before decoding"},
    {"set_skip_after", (PyCFunction)PyDataFormat_set_skip_after, METH_O,
     "set the number of bytes to skip after decoding"},
    {"set_alignment", (PyCFunction)PyDataFormat_set_alignment, METH_O,
     "set the alignment at which decoding starts"},
    {"set_fixed_offset", (PyCFunction)PyDataFormat_set_fixed_offset, METH_O,
     "set an offset from which decoding starts"},
    {"set_tag", (PyCFunction)PyDataFormat_set_tag, METH_O,
     "set an offset from which decoding starts"},
    {"decode", (PyCFunction)PyDataFormat_decode, METH_O,
     "decode the given DataFormat starting from the current offset"},
    {NULL}};

static void PyDataFormat_dealloc(PyDataFormat* self)
{
    if (self->format)
    {
        delete self->format;
    }
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyDataFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._DataFormat"};

static bool registerDataFormat(PyObject* module)
{
    PyDataFormatType.tp_basicsize = sizeof(PyDataFormat);
    PyDataFormatType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyDataFormatType.tp_doc = "Describes the format of a sequence of bytes";
    PyDataFormatType.tp_methods = PyDataFormat_methods;
    PyDataFormatType.tp_dealloc = (destructor)&PyDataFormat_dealloc;
    if (PyType_Ready(&PyDataFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataFormatType);
    PyModule_AddObject(module, "_DataFormat", (PyObject*)&PyDataFormatType);
    return true;
}

//##[ IntegerFormat ]##############################################################################

static int PyIntegerFormat_init(PyIntegerFormat* self, PyObject* args, PyObject* kwargs)
{
    static const char* keywords[] = {"size", "signedness", "endianness", NULL};

    unsigned long long size;
    PyObject* signednessObj{nullptr};
    PyObject* endiannessObj{nullptr};
    size_t row{0}, column{0};
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "KOO", const_cast<char**>(keywords), &size,
                                    &signednessObj, &endiannessObj) < 0)
    {
        return -1;
    }

    fuse::Signedness signedness{fuse::Signedness::Unsigned};
    if (signednessObj)
    {
        if (PyObject_HasAttrString(signednessObj, "value"))
        {
            auto* value = PyObject_GetAttrString(signednessObj, "value");
            if (PyLong_Check(value))
            {
                signedness = static_cast<fuse::Signedness>(PyLong_AsSize_t(value));
            }
            Py_DECREF(value);
        }
        Py_DECREF(signednessObj);
    }

    fuse::Endianness endianness{fuse::Endianness::Little};
    if (endiannessObj)
    {
        if (PyObject_HasAttrString(endiannessObj, "value"))
        {
            auto* value = PyObject_GetAttrString(endiannessObj, "value");
            if (PyLong_Check(value))
            {
                endianness = static_cast<fuse::Endianness>(PyLong_AsSize_t(value));
            }
            Py_DECREF(value);
        }
        Py_DECREF(endiannessObj);
    }

    self->dataFormat.format = new IntegerFormat{static_cast<size_t>(size), signedness, endianness};
    return 0;
}

static PyMethodDef PyIntegerFormat_methods[] = {{NULL}};

PyTypeObject PyIntegerFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._IntegerFormat"};

static bool registerIntegerFormat(PyObject* module)
{
    PyIntegerFormatType.tp_new = PyType_GenericNew;
    PyIntegerFormatType.tp_base = &PyDataFormatType;
    PyIntegerFormatType.tp_basicsize = sizeof(PyIntegerFormat);
    PyIntegerFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyIntegerFormatType.tp_doc = "Describes the binary format of an integer";
    PyIntegerFormatType.tp_methods = PyIntegerFormat_methods;
    PyIntegerFormatType.tp_init = (initproc)PyIntegerFormat_init;
    if (PyType_Ready(&PyIntegerFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyIntegerFormatType);
    PyModule_AddObject(module, "_IntegerFormat", (PyObject*)&PyIntegerFormatType);
    return true;
}

//##[ StringFormat ]###############################################################################

static int PyStringFormat_init(PyIntegerFormat* self, PyObject* args, PyObject* kwargs)
{
    static const char* keywords[] = {"encoding", NULL};

    PyObject* pyEncoding;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "O", const_cast<char**>(keywords), &pyEncoding) <
        0)
    {
        return -1;
    }

    if (!PyObject_IsInstance(pyEncoding, (PyObject*)&Py_TextEncoding))
    {
        PyErr_SetString(PyExc_TypeError, "expected a _TextEncoding");
        return -1;
    }

    self->dataFormat.format =
        new StringFormat{reinterpret_cast<PyTextEncoding*>(pyEncoding)->encoding};
    return 0;
}

static void PyStringFormat_dealloc(PyIntegerFormat* self)
{
    if (self->dataFormat.format)
    {
        delete self->dataFormat.format;
    }
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyStringFormat_methods[] = {{NULL}};

PyTypeObject PyStringFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._StringFormat"};

static bool registerStringFormat(PyObject* module)
{
    PyStringFormatType.tp_new = PyType_GenericNew;
    PyStringFormatType.tp_base = &PyDataFormatType;
    PyStringFormatType.tp_basicsize = sizeof(PyStringFormat);
    PyStringFormatType.tp_dealloc = (destructor)PyStringFormat_dealloc;
    PyStringFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyStringFormatType.tp_doc = "Describes the binary format of a string";
    PyStringFormatType.tp_methods = PyStringFormat_methods;
    PyStringFormatType.tp_init = (initproc)PyStringFormat_init;
    if (PyType_Ready(&PyStringFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyStringFormatType);
    PyModule_AddObject(module, "_StringFormat", (PyObject*)&PyStringFormatType);
    return true;
}

//##[ RecordFormat ]###############################################################################

static int PyRecordFormat_init(PyRecordFormat* self, PyObject* args, PyObject* kwargs)
{
    self->dataFormat.format = new RecordFormat;
    return 0;
}

static auto PyRecordFormat_append_element(PyRecordFormat* self, PyObject* const* args,
                                          const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_TypeError, "wrong number of arguments; expected 2");
        return NULL;
    }

    const char* name = PyUnicode_AsUTF8(args[0]);
    if (!name)
    {
        return NULL;
    }

    if (!PyObject_IsInstance(args[1], (PyObject*)&PyDataFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "expected a _DataFormat");
        return NULL;
    }

    auto elementFormat = reinterpret_cast<PyDataFormat*>(args[1])->format->copy();
    auto* asRecord = static_cast<RecordFormat*>(self->dataFormat.format);
    asRecord->append(name, std::move(elementFormat));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyRecordFormat_methods[] = {
    {"append_element", (PyCFunction)PyRecordFormat_append_element, METH_FASTCALL,
     "append an element format to the Record"},
    {NULL}};

PyTypeObject PyRecordFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._RecordFormat"};

static bool registerRecordFormat(PyObject* module)
{
    PyRecordFormatType.tp_new = PyType_GenericNew;
    PyRecordFormatType.tp_base = &PyDataFormatType;
    PyRecordFormatType.tp_basicsize = sizeof(PyRecordFormat);
    PyRecordFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyRecordFormatType.tp_doc = "Describes the binary format of a record";
    PyRecordFormatType.tp_methods = PyRecordFormat_methods;
    PyRecordFormatType.tp_init = (initproc)PyRecordFormat_init;
    if (PyType_Ready(&PyRecordFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyRecordFormatType);
    PyModule_AddObject(module, "_RecordFormat", (PyObject*)&PyRecordFormatType);
    return true;
}

//##[ ArrayFormat ]###############################################################################

static int PyArrayFormat_init(PyArrayFormat* self, PyObject* args, PyObject* kwargs)
{
    self->dataFormat.format = new ArrayFormat;
    return 0;
}

static auto PyArrayFormat_set_fixed_length(PyArrayFormat* self, PyObject* pyLength) -> PyObject*
{
    auto const length = PyLong_AsUnsignedLongLong(pyLength);
    if (length == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto& asArray = static_cast<ArrayFormat&>(*self->dataFormat.format);
    asArray.setSizeProvider(std::make_unique<FixedSizeProvider>(length));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyArrayFormat_set_element_format(PyArrayFormat* self, PyObject* pyFormat) -> PyObject*
{
    if (!PyObject_IsInstance(pyFormat, (PyObject*)&PyDataFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "expected a _DataFormat");
        return NULL;
    }
    auto elementFormat = reinterpret_cast<PyDataFormat*>(pyFormat)->format->copy();
    auto& asArray = static_cast<ArrayFormat&>(*self->dataFormat.format);
    asArray.setElementFormat(std::move(elementFormat));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyArrayFormat_methods[] = {
    {"set_fixed_length", (PyCFunction)PyArrayFormat_set_fixed_length, METH_O,
     "make the Array a fixed length"},
    {"set_element_format", (PyCFunction)PyArrayFormat_set_element_format, METH_O,
     "set the DataFormat of the elements of this Array"},
    {NULL}};

PyTypeObject PyArrayFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._ArrayFormat"};

static bool registerArrayFormat(PyObject* module)
{
    PyArrayFormatType.tp_new = PyType_GenericNew;
    PyArrayFormatType.tp_base = &PyDataFormatType;
    PyArrayFormatType.tp_basicsize = sizeof(PyArrayFormat);
    PyArrayFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyArrayFormatType.tp_doc = "Describes the binary format of an array";
    PyArrayFormatType.tp_methods = PyArrayFormat_methods;
    PyArrayFormatType.tp_init = (initproc)PyArrayFormat_init;
    if (PyType_Ready(&PyArrayFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyArrayFormatType);
    PyModule_AddObject(module, "_ArrayFormat", (PyObject*)&PyArrayFormatType);
    return true;
}

//##[ ArrayFormat ]###############################################################################

static int PyPointerFormat_init(PyPointerFormat* self, PyObject* args, PyObject* kwargs)
{
    self->dataFormat.format = new PointerFormat;
    return 0;
}

static auto PyPointerFormat_set_pointee_format(PyPointerFormat* self, PyObject* pyFormat)
    -> PyObject*
{
    if (!PyObject_IsInstance(pyFormat, (PyObject*)&PyDataFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "expected a _DataFormat");
        return NULL;
    }
    auto elementFormat = reinterpret_cast<PyDataFormat*>(pyFormat)->format->copy();
    auto& asPointer = static_cast<PointerFormat&>(*self->dataFormat.format);
    asPointer.setPointedFormat(std::move(elementFormat));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyPointerFormat_set_nullpointer(PyPointerFormat* self, PyObject* pyAddress) -> PyObject*
{
    auto const address = PyLong_AsUnsignedLongLong(pyAddress);
    if (address == static_cast<unsigned long long>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto maybeAddress =
        static_cast<PointerFormat*>(self->dataFormat.format)->addressFormat().fromInteger(address);
    if (!maybeAddress)
    {
        PyErr_SetString(PyExc_RuntimeError, "could not convert into Address");
        return NULL;
    }
    static_cast<PointerFormat*>(self->dataFormat.format)->setNullPointer(*maybeAddress);
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyPointerFormat_set_address_layout(PyPointerFormat* self, PyObject* pyLayout)
    -> PyObject*
{
    if (!PyObject_IsInstance(pyLayout, (PyObject*)&PyAddressLayoutType))
    {
        PyErr_SetString(PyExc_TypeError, "expected an _AddressLayout");
        return NULL;
    }
    auto layout = reinterpret_cast<PyAddressLayout*>(pyLayout)->layout->copy();
    auto& asPointer = static_cast<PointerFormat&>(*self->dataFormat.format);
    asPointer.setLayout(std::move(layout));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyPointerFormat_set_address_format(PyPointerFormat* self, PyObject* pyFormat)
    -> PyObject*
{
    if (!PyObject_IsInstance(pyFormat, (PyObject*)&PyAddressFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "expected an _AddressFormat");
        return NULL;
    }
    auto const* format = reinterpret_cast<PyAddressFormat*>(pyFormat)->format;
    auto& asPointer = static_cast<PointerFormat&>(*self->dataFormat.format);
    asPointer.setAddressFormat(const_cast<fuse::AddressFormat*>(format));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyPointerFormat_use_address_map(PyPointerFormat* self, PyObject* pyUseAddressMap)
    -> PyObject*
{
    auto& asPointer = static_cast<PointerFormat&>(*self->dataFormat.format);
    asPointer.useAddressMap(PyObject_IsTrue(pyUseAddressMap) != 0);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyPointerFormat_methods[] = {
    {"set_pointee_format", (PyCFunction)PyPointerFormat_set_pointee_format, METH_O,
     "set the DataFormat of the pointed-at element"},
    {"set_address_layout", (PyCFunction)PyPointerFormat_set_address_layout, METH_O,
     "set the AddressLayout (storage layout) of the address to be decoded"},
    {"set_address_format", (PyCFunction)PyPointerFormat_set_address_format, METH_O,
     "set the AddressFormat (logical structure) of the address to be decoded"},
    {"set_null_pointer", (PyCFunction)PyPointerFormat_set_nullpointer, METH_O,
     "set the address which is considered the null pointer"},
    {"use_address_map", (PyCFunction)PyPointerFormat_use_address_map, METH_O,
     "set whether the decoded Address is filtered through an AddressMap"},
    {NULL}};

PyTypeObject PyPointerFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._PointerFormat"};

static bool registerPointerFormat(PyObject* module)
{
    PyPointerFormatType.tp_new = PyType_GenericNew;
    PyPointerFormatType.tp_base = &PyDataFormatType;
    PyPointerFormatType.tp_basicsize = sizeof(PyPointerFormat);
    PyPointerFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyPointerFormatType.tp_doc = "Describes the binary format of a pointer";
    PyPointerFormatType.tp_methods = PyPointerFormat_methods;
    PyPointerFormatType.tp_init = (initproc)PyPointerFormat_init;
    if (PyType_Ready(&PyPointerFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyPointerFormatType);
    PyModule_AddObject(module, "_PointerFormat", (PyObject*)&PyPointerFormatType);
    return true;
}

//#################################################################################################

static const std::map<std::string, fuse::Signedness> SignednessEnum = {
    {"SIGNED", fuse::Signedness::Signed},
    {"UNSIGNED", fuse::Signedness::Unsigned},
};

static const std::map<std::string, fuse::Endianness> EndiannessEnum = {
    {"LITTLE", fuse::Endianness::Little},
    {"BIG", fuse::Endianness::Big},
};

template <class Enum>
void registerEnumeration(PyObject* module, const std::string& name,
                         const std::map<std::string, Enum>& enumeration)
{
    auto* enumModule = PyImport_ImportModule("enum");
    auto* enumClass = PyObject_GetAttrString(enumModule, "Enum");
    auto* mapping = PyDict_New();
    for (auto const& pair : enumeration)
    {
        PyDict_SetItemString(mapping, pair.first.c_str(),
                             PyLong_FromSize_t(static_cast<size_t>(pair.second)));
    }
    auto* nameUnicode = PyUnicode_FromString(name.c_str());
    auto* tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, nameUnicode);
    PyTuple_SetItem(tuple, 1, mapping);
    auto* theClass = PyObject_CallObject(enumClass, tuple);
    Py_DECREF(tuple);
    Py_DECREF(enumClass);
    Py_DECREF(enumModule);
    PyModule_AddObject(module, name.c_str(), theClass);
}

static bool registerDataFormatEnumerations(PyObject* module)
{
    registerEnumeration(module, "Signedness", SignednessEnum);
    registerEnumeration(module, "Endianness", EndiannessEnum);
    return true;
}

//#################################################################################################

bool registerDataFormatTypes(PyObject* module)
{
    if (!registerDataFormatEnumerations(module))
    {
        return false;
    }
    if (!registerDataFormat(module))
    {
        return false;
    }
    if (!registerIntegerFormat(module))
    {
        return false;
    }
    if (!registerStringFormat(module))
    {
        return false;
    }
    if (!registerRecordFormat(module))
    {
        return false;
    }
    if (!registerArrayFormat(module))
    {
        return false;
    }
    if (!registerPointerFormat(module))
    {
        return false;
    }
    return true;
}
