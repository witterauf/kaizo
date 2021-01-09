#include "pyutilities.h"
#include <kaizo/text/TableEncoding.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace kaizo;

static void Table_insert_text_entry(Table& table, py::buffer buffer, const std::string& text)
{
    auto const view = requestReadOnly(buffer);
    auto const seq = BinarySequence(reinterpret_cast<const char*>(view.data()), view.size());
    auto const entry = TableEntry::makeText(text);
    table.insert(seq, entry);
}

static auto TextEncoding_decode(TextEncoding& encoding, py::buffer b, const size_t offset)
    -> std::pair<size_t, std::string>
{
    auto const view = requestReadOnly(b);
    return encoding.decode(view, offset);
}

static auto decodeParameters(const std::string& format) -> std::vector<TableEntry::ParameterFormat>
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
            throw py::value_error{"invalid format string"};
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

static void Table_insert_control_entry(Table& table, py::buffer buffer, const std::string& label,
                                       const std::optional<std::string>& parameters,
                                       const std::optional<std::string>& suffix)
{
    std::vector<TableEntry::ParameterFormat> parameterFormats;
    if (parameters)
    {
        parameterFormats = decodeParameters(*parameters);
    }
    auto const entry =
        TableEntry::makeControl(TableEntry::Label{label, suffix.value_or("")}, parameterFormats);
    auto const view = requestReadOnly(buffer);
    auto const seq = BinarySequence(reinterpret_cast<const char*>(view.data()), view.size());
    table.insert(seq, entry);
}

static void Table_insert_end_entry(Table& table, py::buffer buffer, const std::string& label,
                                   const std::optional<std::string>& suffix)
{
    auto const view = requestReadOnly(buffer);
    auto const seq = BinarySequence(reinterpret_cast<const char*>(view.data()), view.size());
    auto const entry = TableEntry::makeEnd(TableEntry::Label{label, suffix.value_or("")});
    table.insert(seq, entry);
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

static auto makeTableControlEntry(const TableEntry& entry) -> py::tuple
{
    std::string parameters;
    for (size_t i = 0; i < entry.parameterCount(); ++i)
    {
        auto const& parameter = entry.parameter(i);
        parameters += convert(parameter);
    }

    py::tuple tuple(4);
    tuple[0] = 1;
    tuple[1] = entry.labelName();
    tuple[2] = entry.label().postfix;
    tuple[3] = parameters;
    return tuple;
}

static auto makeTableTextEntry(const TableEntry& entry) -> py::tuple
{
    py::tuple tuple(2);
    tuple[0] = 0;
    tuple[1] = entry.text();
    return tuple;
}

static auto makeTableEndEntry(const TableEntry& entry) -> py::tuple
{
    py::tuple tuple(3);
    tuple[0] = 3;
    tuple[1] = entry.label().name;
    tuple[2] = entry.label().postfix;
    return tuple;
}

static auto makeTableSwitchEntry(const TableEntry& entry) -> py::tuple
{
    py::tuple tuple(2);
    tuple[0] = 2;
    tuple[1] = entry.targetTable();
    return tuple;
}

static auto convert(const TableEntry& entry) -> py::tuple
{
    switch (entry.kind())
    {
    case TableEntry::Kind::Text: return makeTableTextEntry(entry);
    case TableEntry::Kind::Control: return makeTableControlEntry(entry);
    case TableEntry::Kind::End: return makeTableEndEntry(entry);
    case TableEntry::Kind::TableSwitch: return makeTableSwitchEntry(entry);
    }
}

static auto Table_get_entry(const Table& table, const size_t index)
    -> std::pair<py::bytes, py::tuple>
{
    auto const& entry = table.entry(index);
    const py::bytes bytes(entry.binary().data(), entry.binary().size());
    auto const pyEntry = convert(entry.text());
    return std::make_pair(bytes, pyEntry);
}

void registerKaizoText(py::module_& m)
{
    py::class_<Table>(m, "_Table")
        .def(py::init())
        .def("insert_text_entry", &Table_insert_text_entry)
        .def("insert_control_entry", &Table_insert_control_entry)
        .def("insert_end_entry", &Table_insert_end_entry)
        .def("get_entry", &Table_get_entry)
        .def_property_readonly("entry_count", &Table::size);

    py::class_<TextEncoding, std::shared_ptr<TextEncoding>>(m, "_TextEncoding")
        .def("encode",
             [](TextEncoding& encoding, const std::string& text) { return encoding.encode(text); })
        .def("decode", &TextEncoding_decode)
        .def_property_readonly("supports_encoding",
                               [](const TextEncoding& encoding) { return encoding.canEncode(); })
        .def_property_readonly("supports_decoding",
                               [](const TextEncoding& encoding) { return encoding.canDecode(); });

    m.add_object("_AsciiEncoding", py::cast(static_cast<TextEncoding*>(new AsciiEncoding{}),
                                            py::return_value_policy::take_ownership));
}

/*

//##[ TableEncoding
]##############################################################################

static PyMethodDef PyKaizoTableEncoding_methods[] = {{NULL}};

PyTypeObject PyKaizoTableEncodingType = {PyVarObject_HEAD_INIT(NULL, 0)
"_kaizopy._TableEncoding"};

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
        PyObject* args = Py_BuildValue("(y#K)", binary.data(), binary.size(),
(Py_ssize_t)offset); PyObject* result = PyObject_CallObject(m_callable, args); Py_DECREF(args);
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
        PyErr_SetString(PyExc_TypeError, "expected None or a callable object as second
argument"); return -1;
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
*/