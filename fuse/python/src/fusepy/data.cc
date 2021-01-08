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
    case DataType::Null: Py_INCREF(Py_None); return Py_None;
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

auto fromPyUnicode(PyObject* data) -> std::unique_ptr<StringData>
{
    const char* szData = PyUnicode_AsUTF8(data);
    if (!szData)
    {
        return NULL;
    }
    return std::make_unique<StringData>(szData);
}

auto fromPyLong(PyObject* data) -> std::unique_ptr<IntegerData>
{
    int overflow;
    auto const number = PyLong_AsLongLongAndOverflow(data, &overflow);
    if (overflow == 1)
    {
        auto const unsignedNumber = PyLong_AsUnsignedLongLong(data);
        if (unsignedNumber == static_cast<decltype(unsignedNumber)>(-1) && PyErr_Occurred())
        {
            return NULL;
        }
        return std::make_unique<IntegerData>(unsignedNumber);
    }
    else if (overflow == -1)
    {
        PyErr_SetString(PyExc_ValueError,
                        "number too low (only signed 64 bit supported at the moment)");
        return nullptr;
    }
    else
    {
        if (number == -1 && PyErr_Occurred())
        {
            return NULL;
        }
        return std::make_unique<IntegerData>(number);
    }
}

auto fromPyMapping(PyObject* data) -> std::unique_ptr<RecordData>
{
    auto record = std::make_unique<RecordData>();
    PyObject* items = PyMapping_Items(data);
    for (Py_ssize_t i = 0; i < PyList_Size(items); ++i)
    {
        PyObject* item = PyList_GetItem(items, i);
        PyObject* key = PyTuple_GetItem(item, 0);
        PyObject* value = PyTuple_GetItem(item, 1);

        const char* szKey = PyUnicode_AsUTF8(key);
        if (!szKey)
        {
            Py_DECREF(items);
            return NULL;
        }

        auto valueData = fromNativePython(value);
        if (!valueData)
        {
            Py_DECREF(items);
            return NULL;
        }

        record->set(szKey, std::move(valueData));
    }
    Py_DECREF(items);
    return record;
}

auto fromPySequence(PyObject* data) -> std::unique_ptr<ArrayData>
{
    auto array = std::make_unique<ArrayData>(static_cast<size_t>(PySequence_Length(data)));
    for (Py_ssize_t i = 0; i < PySequence_Length(data); ++i)
    {
        PyObject* item = PySequence_GetItem(data, i);
        auto itemData = fromNativePython(item);
        if (!itemData)
        {
            return nullptr;
        }

        array->set(static_cast<size_t>(i), std::move(itemData));
    }
    return array;
}

auto fromNativePython(PyObject* data) -> std::unique_ptr<Data>
{
    if (PyUnicode_Check(data))
    {
        return fromPyUnicode(data);
    }
    else if (PyLong_Check(data))
    {
        return fromPyLong(data);
    }
    else if (PySequence_Check(data))
    {
        return fromPySequence(data);
    }
    else if (PyMapping_Check(data))
    {
        return fromPyMapping(data);
    }
    else if (PyObject_IsInstance(data, (PyObject*)&PyFuseReferenceType))
    {
        return reinterpret_cast<PyFuseReference*>(data)->data->copy();
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "could not convert into Data");
        return nullptr;
    }
}

//##[ Data ]#################################################################################

static int PyFuseReference_init(PyFuseReference* self, PyObject* args, PyObject*)
{
    const char* szPath{nullptr};
    if (PyArg_ParseTuple(args, "s", &szPath) < 0)
    {
        return -1;
    }
    if (!szPath)
    {
        return -1;
    }

    auto const maybePath = DataPath::fromString(szPath);
    if (!maybePath)
    {
        PyErr_SetString(PyExc_ValueError, "not a valid DataPath");
        return -1;
    }

    self->ownsData = true;
    self->data = new ReferenceData{*maybePath};
    return 0;
}

static void PyFuseReference_dealloc(PyFuseReference* self)
{
    if (self->ownsData)
    {
        delete self->data;
    }
    Py_TYPE(self)->tp_free(self);
}

static auto PyFuseReference_repr(PyFuseReference* self) -> PyObject*
{
    auto const asString = "Reference('" + self->data->path().toString() + "')";
    return Py_BuildValue("s#", asString.c_str(), asString.size());
}

static PyMethodDef PyFuseReference_methods[] = {{NULL}};

PyTypeObject PyFuseReferenceType = {PyVarObject_HEAD_INIT(NULL, 0) "_fusepy.Reference"};

static bool registerData(PyObject* module)
{
    PyFuseReferenceType.tp_new = PyType_GenericNew;
    PyFuseReferenceType.tp_basicsize = sizeof(PyFuseReference);
    PyFuseReferenceType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyFuseReferenceType.tp_doc = "Described a reference to another data item";
    PyFuseReferenceType.tp_methods = PyFuseReference_methods;
    PyFuseReferenceType.tp_init = (initproc)&PyFuseReference_init;
    PyFuseReferenceType.tp_dealloc = (destructor)&PyFuseReference_dealloc;
    PyFuseReferenceType.tp_repr = (reprfunc)&PyFuseReference_repr;

    if (PyType_Ready(&PyFuseReferenceType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyFuseReferenceType);
    PyModule_AddObject(module, "Reference", (PyObject*)&PyFuseReferenceType);
    return true;
}

//#################################################################################################

bool registerFuseDataTypes(PyObject* module)
{
    if (!registerData(module))
    {
        return false;
    }
    return true;
}