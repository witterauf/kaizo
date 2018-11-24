#include "LuaAddressLibrary.h"
#include "loaders/LuaArrayFormatLoader.h"
#include "loaders/LuaIntegerFormatLoader.h"
#include "loaders/LuaRecordFormatLoader.h"
#include "loaders/LuaRelativeOffsetFormatLoader.h"
#include "loaders/LuaStringFormatLoader.h"
#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/IntegerFormat.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/binary/LuaWriter.h>
#include <fuse/binary/PointerFormat.h>
#include <fuse/binary/RecordFormat.h>
#include <fuse/binary/StringFormat.h>
#include <sol.hpp>

namespace fuse::binary {

auto loadStringFormat(const sol::table& format, sol::this_state state)
    -> std::unique_ptr<StringFormat>
{
    LuaStringFormatLoader loader;
    if (auto maybeStringFormat = loader.load(format, state))
    {
        return std::move(*maybeStringFormat);
    }
    else
    {
        throw std::runtime_error{"could not construct StringFormat"};
    }
}

auto loadArrayFormat(const sol::table& format, sol::this_state state)
    -> std::unique_ptr<ArrayFormat>
{
    LuaArrayFormatLoader loader;
    if (auto maybeArrayFormat = loader.load(format, state))
    {
        return std::move(*maybeArrayFormat);
    }
    else
    {
        throw std::runtime_error{"could not construct ArrayFormat"};
    }
}

auto loadRecordFormat(const sol::table& format, sol::this_state state)
    -> std::unique_ptr<RecordFormat>
{
    LuaRecordFormatLoader loader;
    if (auto maybeRecordFormat = loader.load(format, state))
    {
        return std::move(*maybeRecordFormat);
    }
    else
    {
        throw std::runtime_error{"could not construct RecordFormat"};
    }
}

auto loadIntegerFormat(const sol::table& format, sol::this_state state)
    -> std::unique_ptr<IntegerFormat>
{
    LuaIntegerFormatLoader loader;
    if (auto maybeIntegerFormat = loader.load(format, state))
    {
        return std::move(*maybeIntegerFormat);
    }
    else
    {
        throw std::runtime_error{"could not construct IntegerFormat"};
    }
}

auto loadRelativeOffsetFormat(const sol::table& format, sol::this_state state)
{
    LuaRelativeOffsetFormatLoader loader;
    if (auto maybePointerFormat = loader.load(format, state))
    {
        return std::move(*maybePointerFormat);
    }
    else
    {
        throw std::runtime_error{"could not construct RelativeOffsetFormat"};
    }
}

auto makeDataReader(const std::string& filename) -> std::unique_ptr<DataReader>
{
    return std::make_unique<DataReader>(filename);
}

auto getDataReaderRanges(const DataReader& reader, sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    auto const& ranges = reader.ranges();
    sol::table rangesTable = lua.create_table();
    for (auto const& range : ranges)
    {
        sol::table rangeTable = lua.create_table();
        rangeTable["address"] = range.second.address;
        rangeTable["size"] = range.second.size;
        rangesTable[range.first.toString()] = rangeTable;
    }
    return rangesTable;
}

void DataReader_setAddressMap(DataReader& reader, const AddressMap* addressMap)
{
    reader.setAddressMap(addressMap->copy());
}

auto openBinaryLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    auto module = lua.create_table();

    openAddressLibrary(module);

    module.new_enum("SIGNEDNESS", "SIGNED", Signedness::Signed, "UNSIGNED", Signedness::Unsigned);
    module.new_enum("ENDIANNESS", "LITTLE", Endianness::Little, "BIG", Endianness::Big);

    module.new_usertype<Data>("Data");
    module.new_usertype<DataReader>(
        "DataReader", "new", sol::factories(&makeDataReader), "set_offset", &DataReader::setOffset,
        "decoded_ranges", &getDataReaderRanges, "set_address_map", &DataReader_setAddressMap);

    module.new_usertype<DataFormat>("DataFormat", "decode", &DataFormat::decode);
    module.new_usertype<StringFormat>("StringFormat", sol::call_constructor, &loadStringFormat,
                                      sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<ArrayFormat>("ArrayFormat", sol::call_constructor, &loadArrayFormat,
                                     sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<RecordFormat>("RecordFormat", sol::call_constructor, &loadRecordFormat,
                                      sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<PointerFormat>("PointerFormat", sol::base_classes,
                                       sol::bases<DataFormat>());
    module.new_usertype<IntegerFormat>("IntegerFormat", sol::call_constructor, &loadIntegerFormat,
                                       sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<RelativeOffsetFormat>("RelativeOffsetFormat", sol::call_constructor,
                                              &loadRelativeOffsetFormat, sol::base_classes,
                                              sol::bases<DataFormat, PointerFormat>());

    module.new_usertype<LuaWriter>("LuaWriter", "new", sol::constructors<LuaWriter()>(), "write",
                                   sol::resolve<std::string(const Data&)>(&LuaWriter::write));

    return module;
}

} // namespace fuse::binary