#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/StringFormat.h>

namespace fuse::binary {

class LuaStringFormatLoader : public LuaDataFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnknownEncoding[] = "LuaStringFormatLoader.UnknownEncoding";
    };

    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<StringFormat>>;
    auto loadTableFormat(const sol::table& format) -> std::optional<std::unique_ptr<StringFormat>>;

private:
    bool loadFixedLength(const sol::table& table, StringFormat& format);

    void reportUnknownEncoding(const std::string& name);

    sol::this_state* m_lua{nullptr};
};

} // namespace fuse::binary