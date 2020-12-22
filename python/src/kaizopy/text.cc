#include "text.h"
#include <kaizo/text/TableEncoding.h>

using namespace kaizo::text;

//##[ TableEncoding ]##############################################################################

static PyMethodDef PyKaizoTableEncoding_methods[] = {{NULL}};

PyTypeObject PyKaizoTableEncodingType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._TableEncoding"};

class PythonMissingDecoder : public MissingDecoder
{
public:
    explicit PythonMissingDecoder(PyObject* callable)
        : m_callable{callable}
    {
        Py_INCREF(m_callable);
    }

    ~PythonMissingDecoder()
    {
        Py_DECREF(m_callable);
    }

    auto decode(const fuse::BinaryView& binary, size_t offset)
        -> std::optional<std::pair<size_t, std::string>> override
    {
        PyObject* args = Py_BuildValue("(y#K)", binary.data(), binary.size(), (Py_ssize_t)offset);
        PyObject* result = PyObject_CallObject(m_callable, args);
        Py_DECREF(args);
        if (!result)
        {
            return {};
        }

        const char* text{nullptr};
        unsigned long long newOffset{0};
        Py_ssize_t length{0};
        if (PyArg_ParseTuple(result, "Ks#", &newOffset, &text, &length) < 0)
        {
            Py_DECREF(result);
            return {};
        }

        std::string textString(text, length);
        Py_DECREF(result);
        return std::make_pair(newOffset, textString);
    }

    auto copy() const -> std::unique_ptr<MissingDecoder>
    {
        return std::make_unique<PythonMissingDecoder>(m_callable);
    }

private:
    PyObject* m_callable{nullptr};
};

static int PyKaizoTableEncoding_init(PyKaizoTableEncoding* self, PyObject* args, PyObject*)
{
    PyObject* pyTable{nullptr};
    PyObject* pyCallable{nullptr};
    if (PyArg_ParseTuple(args, "OO", &pyTable, &pyCallable) < 0)
    {
        return -1;
    }

    if (!PyObject_IsInstance(pyTable, (PyObject*)&PyKaizoTableType))
    {
        PyErr_SetString(PyExc_TypeError, "expected a Table as first argument");
        return -1;
    }
    if (pyCallable != Py_None && !PyCallable_Check(pyCallable))
    {
        PyErr_SetString(PyExc_TypeError, "expected None or a callable object as second argument");
        return -1;
    }

    auto* encoding = new TableEncoding;
    if (pyCallable != Py_None)
    {
        encoding->setMissingDecoder(std::make_unique<PythonMissingDecoder>(pyCallable));
    }
    encoding->addTable(*reinterpret_cast<PyKaizoTable*>(pyTable)->table);

    new (&self->base.encoding) std::shared_ptr<fuse::text::TextEncoding>(encoding);
    return 0;
}

static bool registerKaizoTableEncoding(PyObject* module)
{
    PyKaizoTableEncodingType.tp_new = PyType_GenericNew;
    PyKaizoTableEncodingType.tp_basicsize = sizeof(PyKaizoTableEncoding);
    PyKaizoTableEncodingType.tp_base = &Py_TextEncoding;
    PyKaizoTableEncodingType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoTableEncodingType.tp_doc = "Represents a TextEncoding based on Tables";
    PyKaizoTableEncodingType.tp_methods = PyKaizoTableEncoding_methods;
    PyKaizoTableEncodingType.tp_init = (initproc)PyKaizoTableEncoding_init;
    if (PyType_Ready(&PyKaizoTableEncodingType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyKaizoTableEncodingType);
    if (PyModule_AddObject(module, "_TableEncoding", (PyObject*)&PyKaizoTableEncodingType) < 0)
    {
        Py_DECREF(&PyKaizoTableEncodingType);
        return false;
    }
    return true;
}

//##[ tableiterator ]##############################################################################

/*
struct PyKaizoTableIterator
{
    PyObject_HEAD;
    Table* table;
    size_t index;
};

PyTypeObject PyKaizoTableIteratorType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._TableIterator"};

static PyMethodDef PyKaizoTableIterator_methods[] = {{NULL}};

static auto PyKaizoTableIterator_iter(PyKaizoTableIterator* self) -> PyObject*
{
    Py_INCREF(self);
    return (PyObject*)self;
}

static auto PyKaizoTableIterator_iternext(PyKaizoTableIterator* self) -> PyObject*
{
    if (self->index >= self->table->size())
    {
        return NULL;
    }
    else
    {
        auto const& entry = self->table->entry(self->index);

    }
}

static bool registerKaizoTableIterator(PyObject* module)
{
    PyKaizoTableIteratorType.tp_new = PyType_GenericNew;
    PyKaizoTableIteratorType.tp_basicsize = sizeof(PyKaizoTableIterator);
    PyKaizoTableIteratorType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoTableIteratorType.tp_doc = "Table iterator";
    PyKaizoTableIteratorType.tp_methods = PyKaizoTableIterator_methods;
    PyKaizoTableIteratorType.tp_iter = (getiterfunc)PyKaizoTableIterator_iter;
    PyKaizoTableIteratorType.tp_iternext = (iternextfunc)PyKaizoTableIterator_iternext;
    if (PyType_Ready(&PyKaizoTableIteratorType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyKaizoTableIteratorType);
    if (PyModule_AddObject(module, "_Table", (PyObject*)&PyKaizoTableIteratorType) < 0)
    {
        Py_DECREF(&PyKaizoTableIteratorType);
        return false;
    }
    return true;
}
*/

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
        PyErr_SetString(
            PyExc_ValueError,
            "wrong number of arguments; expected 4 (binary, label, parameters, postfix)");
        return NULL;
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

static auto convert(const TableEntry::ParameterFormat& parameter) -> std::string
{
    std::string string;
    string += (parameter.endianess == TableEntry::ParameterFormat::Endianess::Little) ? "<" : ">";
    string += std::to_string(parameter.size);
    switch (parameter.preferedDisplay)
    {
    case TableEntry::ParameterFormat::Display::Decimal: string += "d"; break;
    case TableEntry::ParameterFormat::Display::Hexadecimal: string += "x"; break;
    case TableEntry::ParameterFormat::Display::Binary: string += "b"; break;
    default: break;
    }
    return string;
}

static auto makeTableControlEntry(const TableEntry& entry) -> PyObject*
{
    std::string parameters;
    for (size_t i = 0; i < entry.parameterCount(); ++i)
    {
        auto const& parameter = entry.parameter(i);
        parameters += convert(parameter);
    }

    return Py_BuildValue("(Ksss)", 1, entry.labelName().c_str(), entry.label().postfix.c_str(),
                         parameters.c_str());
}

static auto makeTableTextEntry(const TableEntry& entry) -> PyObject*
{
    return Py_BuildValue("(Ks)", 0, entry.text());
}

static auto makeTableEndEntry(const TableEntry& entry) -> PyObject*
{
    return Py_BuildValue("(Kss)", 3, entry.label().name.c_str(), entry.label().postfix.c_str());
}

static auto makeTableSwitchEntry(const TableEntry& entry) -> PyObject*
{
    return Py_BuildValue("(Ks)", 3, entry.targetTable().c_str());
}

static auto PyKaizoTable_getentry(PyKaizoTable* self, PyObject* pyIndex) -> PyObject*
{
    auto const index = PyLong_AsUnsignedLongLong(pyIndex);
    if (index == static_cast<decltype(index)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (index >= self->table->size())
    {
        PyErr_SetString(PyExc_IndexError, "invalid entry index");
        return NULL;
    }

    auto const entry = self->table->entry(index);
    PyObject* binary = Py_BuildValue("y#", entry.binary().c_str(), entry.binary().length());
    switch (entry.text().kind())
    {
    case TableEntry::Kind::Text:
        return Py_BuildValue("(NN)", binary, makeTableTextEntry(entry.text()));
    case TableEntry::Kind::Control:
        return Py_BuildValue("(NN)", binary, makeTableControlEntry(entry.text()));
    case TableEntry::Kind::TableSwitch:
        return Py_BuildValue("(NN)", binary, makeTableSwitchEntry(entry.text()));
    case TableEntry::Kind::End:
        return Py_BuildValue("(NN)", binary, makeTableEndEntry(entry.text()));
    default: return NULL;
    }
}

static auto PyKaizoTable_entry_count(PyKaizoTable* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(self->table->size());
}

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

PyTypeObject PyKaizoTableType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._Table"};

static PyMethodDef PyKaizoTable_methods[] = {
    {"insert_text_entry", (PyCFunction)PyKaizoTable_insert_text_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {"insert_control_entry", (PyCFunction)PyKaizoTable_insert_control_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {"insert_end_entry", (PyCFunction)PyKaizoTable_insert_end_entry, METH_FASTCALL,
     "insert an entry into the table"},
    {"get_entry", (PyCFunction)PyKaizoTable_getentry, METH_O, "get the entry with the given index"},
    {NULL}};

PyGetSetDef PyKaizoTable_getsets[] = {
    {"entry_count", (getter)PyKaizoTable_entry_count, NULL, "the number of entries", NULL}, {NULL}};

static bool registerTable(PyObject* module)
{
    PyKaizoTableType.tp_new = PyType_GenericNew;
    PyKaizoTableType.tp_basicsize = sizeof(PyKaizoTable);
    PyKaizoTableType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    PyKaizoTableType.tp_doc = "Represents a mapping from binary sequences to text or control codes";
    PyKaizoTableType.tp_methods = PyKaizoTable_methods;
    PyKaizoTableType.tp_init = (initproc)PyKaizoTable_init;
    PyKaizoTableType.tp_dealloc = (destructor)PyKaizoTable_dealloc;
    PyKaizoTableType.tp_getset = PyKaizoTable_getsets;
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
    if (!registerKaizoTableEncoding(module))
    {
        return false;
    }
    return true;
}
