#include <diagnostics/ConsoleDiagnosticsConsumer.h>
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/text/LuaTableLibrary.h>
#include <fuse/text/LuaTableReader.h>
#include <fuse/text/LuaTableWriter.h>
#include <fuse/text/StringSet.h>
#include <fuse/text/Table.h>
#include <fuse/text/TableDecoder.h>
#include <fuse/text/TableEncoder.h>
#include <fuse/text/TableEntry.h>
#include <fuse/text/TextEncoding.h>
#include <sol.hpp>

namespace fuse::text {

static auto loadTableFromLua(const std::string& filename, sol::this_state state) -> Table
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

static auto saveTableToLua(const Table& table, const std::string& filename)
{
    LuaTableWriter writer;
    writer.save(filename, table);
}

static auto StringSet_getStrings(const StringSet& stringSet, sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    auto stringsTable = lua.create_table();
    auto const strings = stringSet.strings();
    for (auto i = 0U; i < strings.size(); ++i)
    {
        stringsTable.raw_set(i + 1, strings[i]);
    }
    return stringsTable;
}

static auto TextEncoding_decode(TextEncoding& encoding, const Binary& binary, size_t offset,
                                sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    auto [newOffset, text] = encoding.decode(binary, offset);
    auto table = lua.create_table();
    table["offset"] = newOffset;
    table["text"] = text;
    return table;
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
    module.new_usertype<StringSet>("StringSet", "new", sol::constructors<StringSet()>(), "insert",
                                   &StringSet::insert, "get_strings", &StringSet_getStrings);

    module.new_usertype<TextEncoding>("TextEncoding", "decode", &TextEncoding_decode, "encode",
                                      &TextEncoding::encode);

    return module;
}

} // namespace fuse::text