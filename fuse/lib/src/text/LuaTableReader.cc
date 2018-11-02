#include <diagnostics/Contracts.h>
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/text/LuaTableLibrary.h>
#include <fuse/text/LuaTableReader.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::text {

template <class T, class U>
auto LuaTableReader::requireField(const sol::table& table, const U& name) -> std::optional<T>
{
    auto field = table.get<sol::object>(name);
    if (field.valid())
    {
        if (field.is<T>())
        {
            return field.as<T>();
        }
        else
        {
            reportFieldWrongType(name);
        }
    }
    else
    {
        reportFieldMissing(name);
    }
    return {};
}

template <class T, class U>
auto LuaTableReader::readOptionalField(const sol::table& table, const U& name) -> std::optional<T>
{
    auto field = table.get<sol::object>(name);
    if (field.valid())
    {
        if (field.is<T>())
        {
            return field.as<T>();
        }
        else
        {
            reportFieldWrongType(name);
        }
    }
    return {};
}

void LuaTableReader::setReporter(diagnostics::DiagnosticsReporter* reporter)
{
    m_reporter = reporter;
}

bool LuaTableReader::hasReporter() const
{
    return m_reporter != nullptr;
}

auto LuaTableReader::reporter() -> diagnostics::DiagnosticsReporter&
{
    Expects(m_reporter);
    return *m_reporter;
}

auto LuaTableReader::load(const std::filesystem::path& luaFile, sol::this_state state)
    -> std::optional<Table>
{
    sol::state_view lua{state};
    auto result = lua.script_file(luaFile.string());
    if (result.valid())
    {
        if (result.get_type() == sol::type::table)
        {
            return read(result.get<sol::table>());
        }
        else
        {
            reportResultNotATable();
            return {};
        }
    }
    else
    {
        reportResultInvalid();
        return {};
    }
}

auto LuaTableReader::load(const std::filesystem::path& luaFile) -> std::optional<Table>
{
    sol::state lua;
    lua["text"] = openTextLibrary(lua.lua_state());
    return load(luaFile, lua.lua_state());
}

auto LuaTableReader::read(const sol::table& luaTable) -> std::optional<Table>
{
    Table table;
    auto name = luaTable.get_or("name", std::string{});
    if (!name.empty())
    {
        table.setName(name);
    }

    bool success{true};
    for (auto const& element : luaTable)
    {
        if (element.first.is<sol::table>())
        {
            if (auto maybeBinary = readBinary(element.first.as<sol::table>()))
            {
                if (auto maybeEntry = readEntry(element.second))
                {
                    table.insert(*maybeBinary, *maybeEntry);
                    continue;
                }
            }
            success = false;
        }
    }

    if (success)
    {
        return table;
    }
    else
    {
        return {};
    }
}

auto LuaTableReader::readBinary(const sol::table& binary) -> std::optional<BinarySequence>
{
    BinarySequence sequence;
    for (auto i = 0U; i < binary.size(); ++i)
    {
        if (auto maybeByte = binary.get<sol::optional<uint8_t>>(i + 1))
        {
            sequence += static_cast<BinarySequence::value_type>(*maybeByte);
        }
    }
    if (sequence.size() > 0)
    {
        return sequence;
    }
    else
    {
        reportBinaryMustHaveElement();
        return {};
    }
}

auto LuaTableReader::readEntry(const sol::object& entry) -> std::optional<TableEntry>
{
    switch (entry.get_type())
    {
    case sol::type::table: return readSpecial(entry);
    case sol::type::string: return TableEntry::makeText(entry.as<std::string>());
    default: break;
    }

    reportEntryHasWrongType();
    return {};
}

auto LuaTableReader::readSpecial(const sol::table& control) -> std::optional<TableEntry>
{
    if (auto maybeKind = requireField<TableEntry::Kind>(control, "kind"))
    {
        switch (*maybeKind)
        {
        case TableEntry::Kind::Control: return readControl(control);
        case TableEntry::Kind::End: return readEnd(control);
        case TableEntry::Kind::TableSwitch: return readTableSwitch(control);
        default: break;
        }
    }
    return {};
}

auto LuaTableReader::readControl(const sol::table& control) -> std::optional<TableEntry>
{
    if (auto maybeLabel = readLabel(control))
    {
        if (auto maybeParameters = readOptionalField<sol::table>(control, "parameters"))
        {
            std::vector<TableEntry::ParameterFormat> parameters;
            for (auto i = 0U; i < maybeParameters->size(); ++i)
            {
                if (auto maybeParameterTable = requireField<sol::table>(*maybeParameters, i + 1))
                {
                    if (auto maybeFormat = readParameterFormat(*maybeParameterTable))
                    {
                        parameters.push_back(*maybeFormat);
                    }
                }
            }
            return TableEntry::makeControl(*maybeLabel, parameters);
        }
        else
        {
            return TableEntry::makeControl(*maybeLabel, {});
        }
    }
    return {};
}

auto LuaTableReader::readTableSwitch(const sol::table& control) -> std::optional<TableEntry>
{
    if (auto maybeTable = requireField<std::string>(control, "table"))
    {
        return TableEntry::makeTableSwitch(*maybeTable);
    }
    return {};
}

auto LuaTableReader::readEnd(const sol::table& control) -> std::optional<TableEntry>
{
    if (auto maybeLabel = readLabel(control))
    {
        return TableEntry::makeEnd(*maybeLabel);
    }
    return {};
}

auto LuaTableReader::readLabel(const sol::table& special) -> std::optional<TableEntry::Label>
{
    if (auto maybeLabel = requireField<std::string>(special, "label"))
    {
        TableEntry::Label label;
        label.name = *maybeLabel;
        if (auto maybePostfix = special.get<sol::optional<std::string>>("postfix"))
        {
            label.postfix = *maybePostfix;
        }
        return label;
    }
    return {};
}

auto LuaTableReader::readParameterFormat(const sol::table& table)
    -> std::optional<TableEntry::ParameterFormat>
{
    TableEntry::ParameterFormat format;

    if (!has(table, "size"))
    {
        return {};
    }
    if (auto maybeSize = table.get<sol::optional<unsigned int>>("size"))
    {
        format.size = *maybeSize;
    }
    else
    {
        return {};
    }

    if (has(table, "size"))
    {
        if (auto maybeDisplay =
                table.get<sol::optional<TableEntry::ParameterFormat::Display>>("display"))
        {
            format.preferedDisplay = *maybeDisplay;
        }
        else
        {
            return {};
        }
    }

    if (has(table, "endianness"))
    {
        if (auto maybeEndianness =
                table.get<sol::optional<TableEntry::ParameterFormat::Endianess>>("endianness"))
        {
            format.endianess = *maybeEndianness;
        }
        else
        {
            return {};
        }
    }

    return format;
}

bool LuaTableReader::has(const sol::table& table, const std::string& field) const
{
    return table.get<sol::object>(field).valid();
}

//##[ diagnostics ]############################################################

void LuaTableReader::reportResultNotATable()
{
    if (hasReporter())
    {
        reporter().report("script must return a table").level(DiagnosticLevel::Error);
    }
}

void LuaTableReader::reportResultInvalid()
{
    if (hasReporter())
    {
        reporter().report("script did not return a valid value").level(DiagnosticLevel::Error);
    }
}

void LuaTableReader::reportBinaryMustHaveElement()
{
    if (hasReporter())
    {
        reporter()
            .report("a binary sequence must have at least one byte")
            .level(DiagnosticLevel::Error);
    }
}

void LuaTableReader::reportEntryHasWrongType()
{
    if (hasReporter())
    {
        reporter()
            .report("a table entry must be either a string or a table")
            .level(DiagnosticLevel::Error);
    }
}

void LuaTableReader::reportFieldMissing(const std::string& field)
{
    if (hasReporter())
    {
        reporter().report("missing field '%0'").level(DiagnosticLevel::Error).substitute(field);
    }
}

void LuaTableReader::reportFieldWrongType(const std::string& field)
{
    if (hasReporter())
    {
        reporter()
            .report("field '%0' has the wrong type")
            .level(DiagnosticLevel::Error)
            .substitute(field);
    }
}

void LuaTableReader::reportFieldMissing(size_t index)
{
    if (hasReporter())
    {
        reporter()
            .report("missing field '[%0]'")
            .level(DiagnosticLevel::Error)
            .substitute(std::to_string(index));
    }
}

void LuaTableReader::reportFieldWrongType(size_t index)
{
    if (hasReporter())
    {
        reporter()
            .report("field '[%0]' has the wrong type")
            .level(DiagnosticLevel::Error)
            .substitute(std::to_string(index));
    }
}

} // namespace fuse::text