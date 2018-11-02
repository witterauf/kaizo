#include <diagnostics/ConsoleDiagnosticsConsumer.h>
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/text/LuaTableLibrary.h>
#include <fuse/text/LuaTableReader.h>
#include <fuse/text/LuaTableWriter.h>
#include <fuse/text/Table.h>
#include <fuse/text/TableDecoder.h>
#include <fuse/text/TableEncoder.h>
#include <fuse/text/TableEntry.h>
#include <sol.hpp>

namespace fuse::text {

auto loadTableFromLua(const std::string& filename, sol::this_state state) -> Table
{
    LuaTableReader reader;
    diagnostics::ConsoleDiagnosticsConsumer consumer;
    diagnostics::DiagnosticsReporter reporter;
    reporter.registerConsumer(&consumer);
    reader.setReporter(&reporter);
    if (auto maybeTable = reader.load(filename, state))
    {
        return *maybeTable;
    }
    else
    {
        throw std::runtime_error{"could not load table file '" + filename + "'"};
    }
}

auto saveTableToLua(const Table& table, const std::string& filename)
{
    LuaTableWriter writer;
    writer.save(filename, table);
}

auto openTextLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module.new_enum("TABLE_ENTRY_KIND", "CONTROL", TableEntry::Kind::Control, "TABLE_SWITCH",
                    TableEntry::Kind::TableSwitch, "END", TableEntry::Kind::End);
    module.new_enum("DISPLAY", "HEXADECIMAL", TableEntry::ParameterFormat::Display::Hexadecimal,
                    "DECIMAL", TableEntry::ParameterFormat::Display::Decimal, "BINARY",
                    TableEntry::ParameterFormat::Display::Binary);
    module.new_enum("ENDIANNESS", "LITTLE", TableEntry::ParameterFormat::Endianess::Little, "BIG",
                    TableEntry::ParameterFormat::Endianess::Big);

    module.new_usertype<Table>("Table", "load", sol::factories(&loadTableFromLua), "save",
                               &saveTableToLua);

    module.new_usertype<TableDecoder>(
        "TableDecoder", "new", sol::constructors<TableDecoder(), TableDecoder(const Table&)>(),
        "decode", &TableDecoder::decode);

    module.new_usertype<TableEncoder>(
        "TableEncoder", "new", sol::constructors<TableEncoder(), TableEncoder(const Table&)>(),
        "encode", &TableEncoder::encode);

    return module;
}

} // namespace fuse::text