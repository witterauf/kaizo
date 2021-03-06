#include "pyutilities.h"
#include <kaizo/text/AsciiEncoding.h>
#include <kaizo/text/TableEncoding.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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

static void Table_insert_hook_entry(Table& table, py::buffer buffer, const std::string& name)
{
    auto const view = requestReadOnly(buffer);
    auto const seq = BinarySequence(reinterpret_cast<const char*>(view.data()), view.size());
    auto const entry = TableEntry::makeHook(name);
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

static auto makeTableHookEntry(const TableEntry& entry) -> py::tuple
{
    py::tuple tuple(2);
    tuple[0] = 4;
    tuple[1] = entry.hook();
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

static auto makeTableArgument(const TableEntry::ParameterFormat&,
                              const TableEntry::ParameterFormat::argument_t argument) -> py::object
{
    return py::int_(static_cast<long long>(argument));
}

static auto convert(const TableEntry& entry) -> py::tuple
{
    switch (entry.kind())
    {
    case TableEntry::Kind::Text: return makeTableTextEntry(entry);
    case TableEntry::Kind::Control: return makeTableControlEntry(entry);
    case TableEntry::Kind::End: return makeTableEndEntry(entry);
    case TableEntry::Kind::TableSwitch: return makeTableSwitchEntry(entry);
    case TableEntry::Kind::Hook: return makeTableHookEntry(entry);
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

static auto TableEncoding_chunks(const TableEncoding& encoding, const std::string& text) -> py::list
{
    auto const chunks = encoding.makeChunks(text);
    py::list list(chunks.size());
    for (size_t i = 0; i < chunks.size(); ++i)
    {
        auto const& entry = chunks[i].mapping.entry.text();
        py::tuple pyEntry = convert(entry);
        if (entry.kind() == TableEntry::Kind::Control)
        {
            auto const& arguments = chunks[i].mapping.arguments;
            py::list pyArguments(arguments.size());
            for (size_t j = 0; j < arguments.size(); ++j)
            {
                pyArguments[j] = makeTableArgument(entry.parameter(j), arguments[j]);
            }

            py::tuple tuple(4);
            tuple[0] = chunks[i].text;
            tuple[1] = py::bytes(chunks[i].mapping.entry.binary().c_str(),
                                 chunks[i].mapping.entry.binary().size());
            tuple[2] = pyEntry;
            tuple[3] = pyArguments;
            list[i] = tuple;
        }
        else
        {
            py::tuple tuple(3);
            tuple[0] = chunks[i].text;
            tuple[1] = py::bytes(chunks[i].mapping.entry.binary().c_str(),
                                 chunks[i].mapping.entry.binary().size());
            tuple[2] = pyEntry;
            list[i] = tuple;
        }
    }
    return list;
}

static auto TableEncoding_init(const Table& table) -> std::shared_ptr<TableEncoding>
{
    auto encoding = std::make_shared<TableEncoding>();
    encoding->addTable(table);
    return encoding;
}

class PythonHookHandler : public HookHandler
{
public:
    explicit PythonHookHandler(py::object pyDecoder, py::object pyEncoder)
        : m_encoder{pyEncoder}
        , m_decoder{pyDecoder}
    {
    }

    auto decode(const kaizo::BinaryView& binary, size_t offset)
        -> std::optional<std::pair<size_t, std::string>>
    {
        auto const view = py::memoryview::from_memory(binary.data(), binary.size());
        py::object result = m_decoder(view, offset);
        if (!result)
        {
            return {};
        }

        py::tuple tuple = result.cast<py::tuple>();
        auto const newOffset = tuple[0].cast<size_t>();
        auto const text = tuple[1].cast<std::string>();
        return std::make_pair(newOffset, text);
    }

    auto encode(const std::string& name, const std::string& arguments) -> std::optional<Binary>
    {
        py::object result = m_encoder(name, arguments);
        if (!result)
        {
            return {};
        }
        py::buffer buffer = result.cast<py::buffer>();
        auto const view = requestReadOnly(buffer);
        return Binary::from(view);
    }

    auto copy() const -> std::unique_ptr<HookHandler> override
    {
        return nullptr;
    }

private:
    py::object m_encoder;
    py::object m_decoder;
};

static void TableEncoding_add_hook(TableEncoding& encoding, const std::string& name,
                                   py::object pyDecoder, py::object pyEncoder)
{
    auto handler = std::make_shared<PythonHookHandler>(pyDecoder, pyEncoder);
    encoding.addHook(name, handler);
}

void registerKaizoText(py::module_& m)
{
    py::class_<Table>(m, "_Table")
        .def(py::init())
        .def("insert_text_entry", &Table_insert_text_entry)
        .def("insert_control_entry", &Table_insert_control_entry)
        .def("insert_end_entry", &Table_insert_end_entry)
        .def("insert_hook_entry", &Table_insert_hook_entry)
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

    py::class_<TableEncoding, TextEncoding, std::shared_ptr<TableEncoding>>(m, "_TableEncoding")
        .def(py::init(&TableEncoding_init))
        .def("chunks", &TableEncoding_chunks)
        .def("add_hook", &TableEncoding_add_hook);
}
