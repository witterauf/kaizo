#pragma once

#include <fuse/lua/LuaLoader.h>
#include <map>
#include <memory>
#include <optional>
#include <sol_forward.hpp>

namespace fuse::binary {

class DataFormat;

class LuaDataFormatLoader : public lua::LuaLoader
{
protected:
    bool readDataFormat(const sol::table& table, sol::this_state state, DataFormat& format);
};

} // namespace fuse::binary