#include "loaders/LuaArrayFormatLoader.h"
#include "loaders/LuaIntegerFormatLoader.h"
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
#include <fuse/binary/StringFormat.h>
#include <fuse/binary/address_formats/AbsoluteOffset.h>
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

auto makeAbsoluteOffset() -> std::unique_ptr<AbsoluteOffset>
{
    return std::make_unique<AbsoluteOffset>();
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
    module.new_usertype<PointerFormat>("PointerFormat", sol::base_classes,
                                       sol::bases<DataFormat>());
    module.new_usertype<IntegerFormat>("IntegerFormat", sol::call_constructor, &loadIntegerFormat,
                                       sol::base_classes, sol::bases<DataFormat>());
    module.new_usertype<RelativeOffsetFormat>("RelativeOffsetFormat", sol::call_constructor,
                                              &loadRelativeOffsetFormat, sol::base_classes,
                                              sol::bases<DataFormat, PointerFormat>());

    module.new_usertype<AddressFormat>("AddressFormat");
    module.new_usertype<AbsoluteOffset>("AbsoluteOffset", sol::call_constructor,
                                        &makeAbsoluteOffset, sol::base_classes,
                                        sol::bases<AddressFormat>());

    module.new_usertype<LuaWriter>("LuaWriter", "new", sol::constructors<LuaWriter()>(), "write",
                                   sol::resolve<std::string(const Data&)>(&LuaWriter::write));

    return module;
}

} // namespace fuse::binary