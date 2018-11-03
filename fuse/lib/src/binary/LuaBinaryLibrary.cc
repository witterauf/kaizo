#include <fuse/binary/Data.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/IntegerFormat.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/binary/LuaDataFormatLoader.h>
#include <fuse/binary/LuaWriter.h>
#include <sol.hpp>

namespace fuse::binary {

auto loadDataFormat(const sol::table& format, sol::this_state state) -> std::unique_ptr<DataFormat>
{
    LuaDataFormatLoader loader;
    if (auto maybeFormat = loader.load(format, state))
    {
        return std::move(*maybeFormat);
    }
    else
    {
        throw std::runtime_error{"could not load data format description"};
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
    module.new_usertype<DataReader>("DataReader", "new", sol::factories(&makeDataReader));
    module.new_usertype<DataFormat>("DataFormat", "load", sol::factories(&loadDataFormat), "decode",
                                    &DataFormat::decode);
    module.new_usertype<LuaWriter>("LuaWriter", "new", sol::constructors<LuaWriter()>(), "write",
                                   sol::resolve<std::string(const Data&)>(&LuaWriter::write));

    return module;
}

} // namespace fuse::binary