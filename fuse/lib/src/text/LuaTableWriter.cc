#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/text/LuaTableWriter.h>

namespace fuse::text {

void LuaTableWriter::save(const std::filesystem::path& filename, const Table& table)
{
    std::ofstream output{filename};
    if (output.good())
    {
        output << writeTable(table);
    }
    else
    {
        throw std::runtime_error{"could not open file for writing: '" + filename.string() + "'"};
    }
}

auto LuaTableWriter::writeTable(const Table& table) -> std::string
{
    std::string lua;
    lua += "return {\n";
    if (!table.name().empty())
    {
        lua += "  name = \"" + table.name() + "\",\n";
    }
    for (auto i = 0U; i < table.size(); ++i)
    {
        auto const& entry = table.entry(i);
        lua += writeBinary(entry.binary());
        lua += " = ";
        lua += writeTableEntry(entry.text());
        lua += ",\n";
    }
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeBinary(const BinarySequence& binary) -> std::string
{
    std::string lua;
    lua += "{";
    for (auto i = 0U; i < binary.size(); ++i)
    {
        if (i > 0)
        {
            lua += ",";
        }
        lua += "0x";
    }
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeTableEntry(const TableEntry& entry) -> std::string
{
    switch (entry.kind())
    {
    case TableEntry::Kind::Control: return writeControl(entry);
    case TableEntry::Kind::End: return writeEnd(entry);
    case TableEntry::Kind::TableSwitch: return writeTableSwitch(entry);
    case TableEntry::Kind::Text: return writeText(entry);
    default: InvalidCase(entry.kind());
    }
}

auto LuaTableWriter::writeText(const TableEntry& entry) -> std::string
{
    return "[[" + entry.text() + "]]";
}

auto LuaTableWriter::writeControl(const TableEntry& entry) -> std::string
{
    std::string lua;
    lua += "Control{\n";
    lua += writeLabel(entry.label());
    if (entry.parameterCount() > 0)
    {
        lua += "parameters = {\n";
        for (auto i = 0U; i < entry.parameterCount(); ++i)
        {
            lua += writeParameter(entry.parameter(i));
            lua += ",\n";
        }
        lua += "}\n";
    }
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeParameter(const TableEntry::ParameterFormat& format) -> std::string
{
    std::string lua;
    lua += "{\n";
    lua += "  size = " + std::to_string(format.size) + ",\n";
    if (format.endianess != TableEntry::ParameterFormat::Endianess::Little)
    {
        lua += "  endianness = text.ENDIANNESS.BIG,\n";
    }
    switch (format.preferedDisplay)
    {
    case TableEntry::ParameterFormat::Display::Hexadecimal: break;
    case TableEntry::ParameterFormat::Display::Decimal:
        lua += "  display = text.DISPLAY.DECIMAL,\n";
        break;
    case TableEntry::ParameterFormat::Display::Binary:
        lua += "  display = text.DISPLAY.BINARY,\n";
        break;
    default: break;
    }
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeEnd(const TableEntry& entry) -> std::string
{
    std::string lua;
    lua += "End{\n";
    lua += writeLabel(entry.label());
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeTableSwitch(const TableEntry& entry) -> std::string
{
    std::string lua;
    lua += "TableSwitch{\n";
    lua += writeLabel(entry.label());
    lua += "  table = \"" + entry.targetTable() + "\n";
    lua += "}";
    return lua;
}

auto LuaTableWriter::writeLabel(const TableEntry::Label& label) -> std::string
{
    std::string lua;
    lua += "label = \"" + label.name + "\",\n";
    if (!label.postfix.empty())
    {
        lua += "postfix = \"" + label.postfix + "\",\n";
    }
    return lua;
}

} // namespace fuse::text