#include "text.h"

using namespace kaizo::text;

//##[ Table ]######################################################################################

static auto toBinarySequence(PyObject* data) -> std::optional<BinarySequence>
{
    if (PyObject_CheckBuffer(data))
    {
        Py_buffer buffer;
        if (PyObject_GetBuffer(data, &buffer, PyBUF_SIMPLE | PyBUF_C_CONTIGUOUS) < 0)
        {
            return {};
        }
        const char* bufferData = reinterpret_cast<const char*>(buffer.buf);
        auto const size = buffer.len;
        auto binary = BinarySequence(bufferData, size);
        PyBuffer_Release(&buffer);
        return binary;
    }
    else if (PySequence_Check(data))
    {
        BinarySequence sequence;
        auto const length = PySequence_Size(data);
        for (Py_ssize_t i = 0; i < length; ++i)
        {
            auto* item = PySequence_GetItem(data, i);
            auto const byte = PyLong_AsUnsignedLong(item);
            if (byte == static_cast<unsigned long>(-1) && PyErr_Occurred())
            {
                Py_DECREF(item);
                return {};
            }
            if (byte >= 256)
            {
                PyErr_SetString(PyExc_ValueError, "only values from 0 to 255 accepted");
                return {};
            }
            sequence.push_back(static_cast<char>(byte));
            Py_DECREF(item);
        }
        return sequence;
    }
    else if (PyLong_Check(data))
    {
        auto const byte = PyLong_AsUnsignedLong(data);
        if (byte == static_cast<unsigned long>(-1) && PyErr_Occurred())
        {
            return {};
        }
        if (byte >= 256)
        {
            PyErr_SetString(PyExc_ValueError, "only values from 0 to 255 accepted");
            return {};
        }
        return BinarySequence{static_cast<char>(byte)};
    }
    else
    {
        PyErr_SetString(
            PyExc_TypeError,
            "expected an integer, a sequence or an object supporting the buffer protocol");
        return {};
    }
}

static auto toString(PyObject* pyString) -> std::optional<std::string>
{
    const char* text = PyUnicode_AsUTF8(pyString);
    if (!text)
    {
        return {};
    }
    return std::string{text};
}

static auto PyKaizoTable_insert_text_entry(PyKaizoTable* self, PyObject* const* args,
                                           const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 2");
    }
    if (!PyUnicode_Check(args[1]))
    {
        PyErr_SetString(PyExc_TypeError, "expected a str as second argument");
        return NULL;
    }

    auto const text = toString(args[1]);
    if (!text)
    {
        return NULL;
    }

    auto const entry = TableEntry::makeText(*text);
    if (auto const binary = toBinarySequence(args[0]))
    {
        self->table->insert(*binary, entry);
    }
    else
    {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static auto decodeParameters(const std::string& format)
    -> std::optional<std::vector<TableEntry::ParameterFormat>>
{
    using Parameter = TableEntry::ParameterFormat;

    Parameter::Endianess endianess{Parameter::Endianess::Little};
    std::vector<Parameter> parameters;
    for (size_t i{0}; i < format.length();)
    {
        Parameter parameter;

        if (format[i] == '<')
        {
            endianess = Parameter::Endianess::Little;
            ++i;
        }
        else if (format[i] == '>')
        {
            endianess = Parameter::Endianess::Big;
            ++i;
        }
        parameter.endianess = endianess;

        if (format[i] < '0' || format[i] > '9')
        {
            PyErr_SetString(PyExc_ValueError, "invalid format string");
            return {};
        }
        parameter.size = format[i++] - '0';

        if (format[i] == 'd')
        {
            parameter.preferedDisplay = Parameter::Display::Hexadecimal;
            ++i;
        }
        else if (format[i] == 'b')
        {
            parameter.preferedDisplay = Parameter::Display::Hexadecimal;
            ++i;
        }
        else
        {
            parameter.preferedDisplay = Parameter::Display::Hexadecimal;
            if (format[i] == 'x')
            {
                ++i;
            }
        }

        parameters.push_back(parameter);
    }
    return parameters;
}

static auto PyKaizoTable_insert_control_entry(PyKaizoTable* self, PyObject* const* args,
                                              const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 4)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 4");
    }
    if (!PyUnicode_Check(args[1]))
    {
        PyErr_SetString(PyExc_TypeError, "expected a str as second argument");
        return NULL;
    }
    if (args[2] != Py_None && !PyUnicode_Check(args[2]))
    {
        PyErr_SetString(PyExc_TypeError, "expected None or a str as third argument");
        return NULL;
    }
    if (args[3] != Py_None && !PyUnicode_Check(args[3]))
    {
        PyErr_SetString(PyExc_TypeError, "expected None or a str as fourth argument");
        return NULL;
    }

    auto const maybeLabel = toString(args[1]);
    if (!maybeLabel)
    {
        return NULL;
    }

    std::vector<TableEntry::ParameterFormat> parameters;
    if (args[2] != Py_None)
    {
        auto const maybeParameterFormat = toString(args[2]);
        if (!maybeParameterFormat)
        {
            return NULL;
        }
        auto maybeParameters = decodeParameters(*maybeParameterFormat);
        if (!maybeParameters)
        {
            return NULL;
        }
        std::swap(parameters, *maybeParameters);
    }

    std::string suffix;
    if (args[3] != Py_None)
    {
        auto maybeSuffix = toString(args[3]);
        if (!maybeSuffix)
        {
            return NULL;
        }
        std::swap(suffix, *maybeSuffix);
    }

    auto const entry = TableEntry::makeControl(TableEntry::Label{*maybeLabel, suffix}, parameters);

    if (auto const binary = toBinarySequence(args[0]))
    {
        self->table->insert(*binary, entry);
    }
    else
    {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyKaizoTable_insert_end_entry(PyKaizoTable* self, PyObject* const* args,
                                          const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_ValueError, "wrong number of arguments; expected 2");
    }
    if (!PyUnicode_Check(args[1]))
    {
        PyErr_SetString(PyExc_TypeError, "expected a str as second argument");
        return NULL;
    }

    auto const maybeLabel = toString(args[1]);
    if (!maybeLabel)
    {
        return NULL;
    }

    auto const entry = TableEntry::makeEnd(TableEntry::Label{*maybeLabel, ""});

    if (auto const binary = toBinarySequence(args[0]))
    {
        self->table->insert(*binary, entry);
    }
    else
    {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyKaizoTable_methods[] = {
    {"insert_text_entry", (PyCFunction)PyKaizoTable_insert_text_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {"insert_control_entry", (PyCFunction)PyKaizoTable_insert_control_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {"insert_end_entry", (PyCFunction)PyKaizoTable_insert_end_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {NULL}};

PyTypeObject PyKaizoTableType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._Table"};

static int PyKaizoTable_init(PyKaizoTable* self, PyObject*, PyObject*)
{
    self->table = new Table;
    return 0;
}

static void PyKaizoTable_dealloc(PyKaizoTable* self)
{
    delete self->table;
    Py_TYPE(self)->tp_free(self);
}

static bool registerTable(PyObject* module)
{
    PyKaizoTableType.tp_new = PyType_GenericNew;
    PyKaizoTableType.tp_basicsize = sizeof(PyKaizoTable);
    PyKaizoTableType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyKaizoTableType.tp_doc = "Represents a mapping from binary sequences to text or control codes";
    PyKaizoTableType.tp_methods = PyKaizoTable_methods;
    PyKaizoTableType.tp_init = (initproc)PyKaizoTable_init;
    PyKaizoTableType.tp_dealloc = (destructor)PyKaizoTable_dealloc;
    if (PyType_Ready(&PyKaizoTableType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyKaizoTableType);
    if (PyModule_AddObject(module, "_Table", (PyObject*)&PyKaizoTableType) < 0)
    {
        Py_DECREF(&PyKaizoTableType);
        return false;
    }
    return true;
}

auto makeTable(kaizo::text::Table* table) -> PyKaizoTable*
{
    auto* pyTable = PyObject_New(PyKaizoTable, &PyKaizoTableType);
    pyTable->table = table;
    return pyTable;
}

//#################################################################################################

bool registerKaizoText(PyObject* module)
{
    if (!registerTable(module))
    {
        return false;
    }
    return true;
}
