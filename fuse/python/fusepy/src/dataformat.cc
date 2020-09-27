#include "dataformat.h"
#include <iostream>
#include <map>

using namespace fuse::binary;

//##[ DataFormat ]#################################################################################

static PyMethodDef PyDataFormat_methods[] = {{NULL}};

PyTypeObject PyDataFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "fusepy.DataFormat"};

static int PyDataFormat_set_values(DataFormat& format, PyObject* args, PyObject* kwargs)
{
    static const char* keywords[] = {"fixed_offset", "alignment", "skip_before",
                                     "skip_after",   "tag",       NULL};

    PyObject* fixedOffsetObj{nullptr};
    PyObject* alignmentObj{nullptr};
    PyObject* skipBeforeObj{nullptr};
    PyObject* skipAfterObj{nullptr};
    const char* tag{nullptr};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|$OOOOs", const_cast<char**>(keywords),
                                     &fixedOffsetObj, &alignmentObj, &skipBeforeObj, &skipAfterObj,
                                     &tag))
    {
        std::cout << "blub";
        return -1;
    }

    if (fixedOffsetObj)
    {
        if (PyLong_Check(fixedOffsetObj))
        {
            auto const fixedOffset = PyLong_AsUnsignedLongLong(fixedOffsetObj);
            format.setFixedOffset(fixedOffset);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "fixed_offset must be an integer");
            return -1;
        }
    }

    if (alignmentObj)
    {
        if (PyLong_Check(alignmentObj))
        {
            auto const alignment = PyLong_AsUnsignedLongLong(alignmentObj);
            if (alignment != 0)
            {
                format.setAlignment(alignment);
            }
            else
            {
                PyErr_SetString(PyExc_ValueError, "alignment must not be 0");
                return -1;
            }
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "alignment must be an integer");
            return -1;
        }
    }

    if (skipBeforeObj)
    {
        if (PyLong_Check(skipBeforeObj))
        {
            auto const value = PyLong_AsUnsignedLongLong(skipBeforeObj);
            format.setSkipBefore(value);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "skip_before must be an integer");
            return -1;
        }
    }

    if (skipAfterObj)
    {
        if (PyLong_Check(skipAfterObj))
        {
            auto const value = PyLong_AsUnsignedLongLong(skipAfterObj);
            format.setSkipAfter(value);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "skip_after must be an integer");
            return -1;
        }
    }

    if (tag)
    {
        format.setTag(tag);
    }

    return 0;
}

static bool registerDataFormat(PyObject* module)
{
    PyDataFormatType.tp_basicsize = sizeof(PyDataFormat);
    PyDataFormatType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyDataFormatType.tp_doc = "Describes the format of a sequence of bytes";
    PyDataFormatType.tp_methods = PyDataFormat_methods;
    if (PyType_Ready(&PyDataFormatType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataFormatType);
    PyModule_AddObject(module, "DataFormat", (PyObject*)&PyDataFormatType);
    return true;
}

//##[ IntegerFormat ]##############################################################################

static auto splitKeywordArgs(PyObject* kwargs, const char** keywords)
    -> std::optional<std::pair<PyObject*, PyObject*>>
{
    PyObject *key{nullptr}, *value{nullptr};
    Py_ssize_t pos{0};

    PyObject* matches = PyDict_New();
    PyObject* remaining = PyDict_New();

    while (PyDict_Next(kwargs, &pos, &key, &value))
    {
        if (PyUnicode_Check(key))
        {
            const char** keyword = keywords;
            bool found{false};
            while (*keyword != nullptr)
            {
                if (PyUnicode_CompareWithASCIIString(key, *keyword) == 0)
                {
                    found = true;
                    break;
                }
                ++keyword;
            }
            if (found)
            {
                PyDict_SetItem(matches, key, value);
            }
            else
            {
                PyDict_SetItem(remaining, key, value);
            }
        }
        else
        {
            Py_DECREF(matches);
            Py_DECREF(remaining);
            return {};
        }
    }

    return std::make_pair(matches, remaining);
}

static int PyIntegerFormat_init(PyIntegerFormat* self, PyObject* args, PyObject* kwargs)
{
    static const char* keywords[] = {"size", "signedness", "endianness", NULL};

    auto maybeSplitKeywords = splitKeywordArgs(kwargs, keywords);
    if (!maybeSplitKeywords)
    {
        PyErr_SetString(PyExc_TypeError, "only string keys allowed in kwargs");
        return -1;
    }

    unsigned long long size;
    PyObject* signednessObj{nullptr};
    PyObject* endiannessObj{nullptr};
    size_t row{0}, column{0};
    if (!PyArg_ParseTupleAndKeywords(args, maybeSplitKeywords->first, "K|$OO",
                                     const_cast<char**>(keywords), &size, &signednessObj,
                                     &endiannessObj))
    {
        Py_DECREF(maybeSplitKeywords->first);
        Py_DECREF(maybeSplitKeywords->second);
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

    Py_DECREF(maybeSplitKeywords->first);
    if (PyDataFormat_set_values(*self->dataFormat.format, args, maybeSplitKeywords->second) == -1)
    {
        Py_DECREF(maybeSplitKeywords->second);
        return -1;
    }

    Py_DECREF(maybeSplitKeywords->second);
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

static bool registerIntegerFormat(PyObject* module)
{
    PyIntegerFormatType.tp_new = PyType_GenericNew;
    PyIntegerFormatType.tp_base = &PyDataFormatType;
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

//##[ StringFormat ]###############################################################################

static int PyStringFormat_init(PyIntegerFormat* self, PyObject* args, PyObject* kwargs)
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

static void PyStringFormat_dealloc(PyIntegerFormat* self)
{
    if (self->dataFormat.format)
    {
        delete self->dataFormat.format;
    }
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyStringFormat_methods[] = {{NULL}};

PyTypeObject PyStringFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "fusepy.StringFormat"};

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
    PyModule_AddObject(module, "StringFormat", (PyObject*)&PyStringFormatType);
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
    /*
    if (!registerStringFormat(module))
    {
        return false;
    }
    */

    return true;
}