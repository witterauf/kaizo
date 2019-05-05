#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/PointerFormat.h>

namespace fuse::binary {

class LuaRelativeOffsetFormatLoader : public LuaDataFormatLoader
{
public:
    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<PointerFormat>>;

private:
    bool loadLayout(const sol::table& table, PointerFormat& format);
    bool loadAddressFormat(const sol::table& table, PointerFormat& format);
    bool loadPointeeFormat(const sol::table& table, PointerFormat& format);
    bool loadUseAddressMap(const sol::table& table, PointerFormat& format);

    const AddressFormat* m_addressFormat{nullptr};
};

} // namespace fuse::binary