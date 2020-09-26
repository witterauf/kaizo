#include "dataformat.h"
#include <iostream>
#include <map>

using namespace fuse::binary;

//##[ IntegerFormat ]##############################################################################

static int PyIntegerFormat_init(PyIntegerFormat* self, PyObject* args, PyObject* kwargs)
{
    static const char* keywords[] = {"size", "signedness", "endianness", NULL};

    unsigned long long size;
    PyObject* signednessObj;
    PyObject* endiannessObj;
    size_t row{0}, column{0};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$KOO", const_cast<char**>(keywords), &size,
                                     &signednessObj, &endiannessObj))
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

static void PyIntegerFormat_dealloc(PyIntegerFormat* self)
{
    if (self->dataFormat.format)
    {
        delete self->dataFormat.format;
    }
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyIntegerFormat_methods[] = {{NULL}};

PyTypeObject PyIntegerFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "fusepy.IntegerFormat"};

PyTypeObject PySignednessType = {PyVarObject_HEAD_INIT(NULL, 0) "fusepy.Signedness"};

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

//#################################################################################################

bool registerDataFormatTypes(PyObject* module)
{
    registerEnumeration(module, "Signedness", SignednessEnum);
    registerEnumeration(module, "Endianness", EndiannessEnum);

    PyIntegerFormatType.tp_new = PyType_GenericNew;
    PyIntegerFormatType.tp_basicsize = sizeof(PyIntegerFormat);
    PyIntegerFormatType.tp_dealloc = (destructor)PyIntegerFormat_dealloc;
    PyIntegerFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyIntegerFormatType.tp_doc = "Describes the binary format of an integer";
    PyIntegerFormatType.tp_methods = PyIntegerFormat_methods;
    PyIntegerFormatType.tp_init = (initproc)PyIntegerFormat_init;
    if (PyType_Ready(&PyIntegerFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyIntegerFormatType);
    PyModule_AddObject(module, "IntegerFormat", (PyObject*)&PyIntegerFormatType);

    return true;
}