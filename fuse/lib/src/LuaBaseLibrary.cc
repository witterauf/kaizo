#include <fuse/Binary.h>
#include <fuse/Integers.h>
#include <fuse/LuaBaseLibrary.h>
#include <fuse/lua/Utilities.h>
#include <fuse/utilities/CsvReader.h>
#include <fuse/utilities/StringCollection.h>
#include <sol.hpp>

namespace fuse {

auto loadBinary(const std::string& filename) -> Binary
{
    return Binary::load(filename);
}

void saveBinary(const Binary& binary, const std::string& filename)
{
    binary.save(filename);
}

auto loadCsvFile(const std::string& filename, const sol::table& options, sol::this_state s)
    -> sol::table
{
    CsvReader reader{filename};
    if (hasField(options, "columns"))
    {
        auto const columns = readField<sol::table>(options, "columns");
        auto const columnCount = columns.size();
        for (auto i = 0U; i < columnCount; ++i)
        {
            auto const name = readField<std::string>(columns, i + 1);
            reader.setColumnName(i, name);
        }
    }
    sol::state_view lua{s};
    sol::table csvData = lua.create_table();
    unsigned r{0};
    while (auto maybeRow = reader.nextRow())
    {
        auto const& row = *maybeRow;
        auto rowData = lua.create_table();
        for (auto i = 0U; i < row.size(); ++i)
        {
            if (reader.hasColumnName(i))
            {
                rowData[reader.columnName(i)] = row[i];
            }
            else
            {
                rowData[i + 1] = row[i];
            }
        }
        csvData[r + 1] = rowData;
        ++r;
    }
    return csvData;
}

auto StringCollection_strings(const StringCollection& collection, sol::this_state s) -> sol::table
{
    sol::state_view lua{s};
    sol::table table = lua.create_table();
    auto const strings = collection.strings();
    for (auto i = 0U; i < strings.size(); ++i)
    {
        table[i + 1] = strings[i];
    }
    return table;
}

auto openBaseLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module.new_enum("SIGNEDNESS", "SIGNED", Signedness::Signed, "UNSIGNED", Signedness::Unsigned);
    module.new_enum("ENDIANNESS", "LITTLE", Endianness::Little, "BIG", Endianness::Big);
    module.new_usertype<Binary>("Binary", "load", sol::factories(&loadBinary), "save", &saveBinary);
    module.new_usertype<StringCollection>("StringCollection", "insert", &StringCollection::insert,
                                          "strings", StringCollection_strings);
    module["loadcsv"] = &loadCsvFile;
    return module;
}

} // namespace fuse