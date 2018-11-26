#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/RelativePointerFormat.h>

namespace fuse::binary {

class LuaRelativeOffsetFormatLoader : public LuaDataFormatLoader
{
public:
    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<RelativeOffsetFormat>>;

private:
    bool loadOffsetFormat(const sol::table& table, RelativeOffsetFormat& format);
    bool loadAddressFormat(const sol::table& table, RelativeOffsetFormat& format);
    bool loadBaseAddress(const sol::table& table, RelativeOffsetFormat& format);
    bool loadPointeeFormat(const sol::table& table, RelativeOffsetFormat& format);
    bool loadNullPointer(const sol::table& table, RelativeOffsetFormat& format);
    bool loadUseAddressMap(const sol::table& table, RelativeOffsetFormat& format);
};

} // namespace fuse::binary