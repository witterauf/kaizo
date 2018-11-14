#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/RecordFormat.h>

namespace fuse::binary {

class LuaRecordFormatLoader : public LuaDataFormatLoader
{
public:
    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<RecordFormat>>;

private:
    bool loadElements(const sol::table& table, RecordFormat& format);
    bool loadElement(const sol::table& table, RecordFormat& format);
};

} // namespace fuse::binary