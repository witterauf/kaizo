#include "loaders/LuaStringFormatLoader.h"
#include "loaders/LuaArrayFormatLoader.h"
#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/IntegerFormat.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/binary/LuaWriter.h>
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

auto makeDataReader(const std::string& filename) -> std::unique_ptr<DataReader>
{
    return std::make_unique<DataReader>(filename);
}

auto openBinaryLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    auto module = lua.create_table();

    module.new_enum("SIGNEDNESS", "SIGNED", Signedness::Signed, "UNSIGNED", Signedness::Unsigned);
    module.new_enum("ENDIANNESS", "LITTLE", Endianness::Little, "BIG", Endianness::Big);

    module.new_usertype<Data>("Data");
    module.new_usertype<DataReader>("DataReader", "new", sol::factories(&makeDataReader),
                                    "set_offset", &DataReader::setOffset);
    module.new_usertype<DataFormat>("DataFormat", "decode", &DataFormat::decode);
    module.new_usertype<StringFormat>("StringFormat", sol::call_constructor, &loadStringFormat,
                                      sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<ArrayFormat>("ArrayFormat", sol::call_constructor, &loadArrayFormat,
                                     sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<LuaWriter>("LuaWriter", "new", sol::constructors<LuaWriter()>(), "write",
                                   sol::resolve<std::string(const Data&)>(&LuaWriter::write));

    return module;
}

} // namespace fuse::binary