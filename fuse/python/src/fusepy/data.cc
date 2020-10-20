#include "data.h"

using namespace fuse::binary;

auto toNativePython(const fuse::binary::Data& data) -> PyObject*
{
    switch (data.type())
    {
    case DataType::Integer: return toNativePython(static_cast<const IntegerData&>(data));
    case DataType::String: return toNativePython(static_cast<const StringData&>(data));
    case DataType::Array: return toNativePython(static_cast<const ArrayData&>(data));
    case DataType::Record: return toNativePython(static_cast<const RecordData&>(data));
    default: PyErr_SetString(PyExc_ValueError, "unsupported Data"); return NULL;
    }
}

auto toNativePython(const fuse::binary::IntegerData& data) -> PyObject*
{
    if (data.isNegative())
    {
        return PyLong_FromLongLong(data.asSigned());
    }
    else
    {
        return PyLong_FromUnsignedLongLong(data.asUnsigned());
    }
}

auto toNativePython(const fuse::binary::StringData& data) -> PyObject*
{
    return PyUnicode_FromStringAndSize(data.value().c_str(), data.value().length());
}

auto toNativePython(const fuse::binary::ArrayData& array) -> PyObject*
{
    auto* pyList = PyList_New(array.elementCount());
    for (size_t i = 0; i < array.elementCount(); ++i)
    {
        auto* pyElement = toNativePython(array.element(i));
        if (!pyElement)
        {
            Py_DECREF(pyList);
            return NULL;
        }
        PyList_SetItem(pyList, static_cast<Py_ssize_t>(i), pyElement);
    }
    return pyList;
}

auto toNativePython(const fuse::binary::RecordData& record) -> PyObject*
{
    auto* pyDict = PyDict_New();
    auto const names = record.elementNames();
    for (auto const& name : names)
    {
        auto* pyElement = toNativePython(record.element(name));
        if (!pyElement)
        {
            Py_DECREF(pyDict);
            return NULL;
        }
        auto* pyKey = PyUnicode_FromStringAndSize(name.c_str(), name.length());
        PyDict_SetItem(pyDict, pyKey, pyElement);
    }
    return pyDict;
}

/*

//##[ Data ]#################################################################################

static PyMethodDef PyData_methods[] = {{NULL}};

static void PyData_dealloc(PyData* self)
{
    if (self->ownsData)
    {
        delete self->data;
    }
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyDataType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._Data"};

static bool registerData(PyObject* module)
{
    PyDataType.tp_basicsize = sizeof(PyData);
    PyDataType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyDataType.tp_doc = "Describes decoded binary data";
    PyDataType.tp_methods = PyData_methods;
    PyDataType.tp_dealloc = (destructor)&PyData_dealloc;
    if (PyType_Ready(&PyDataType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyDataType);
    PyModule_AddObject(module, "_Data", (PyObject*)&PyDataType);
    return true;
}

//##[ IntegerData ]################################################################################

static PyMethodDef PyIntegerData_methods[] = {{NULL}};

PyTypeObject PyIntegerDataType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy._IntegerData"};

static bool registerIntegerData(PyObject* module)
{
    PyIntegerDataType.tp_new = PyType_GenericNew;
    PyIntegerDataType.tp_base = &PyDataType;
    PyIntegerDataType.tp_basicsize = sizeof(PyIntegerData);
    PyIntegerDataType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyIntegerDataType.tp_doc = "Describes an integer decoded from binary data";
    PyIntegerDataType.tp_methods = PyIntegerData_methods;
    if (PyType_Ready(&PyIntegerDataType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyIntegerDataType);
    PyModule_AddObject(module, "_IntegerData", (PyObject*)&PyIntegerDataType);
    return true;
}

//#################################################################################################

bool registerFuseData(PyObject* module)
{
    return true;
}
*/