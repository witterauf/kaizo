#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/ArrayFormat.h>

namespace fuse::binary {

class LuaArrayFormatLoader : public LuaDataFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char InvalidSize[] = "LuaArrayFormatLoader.InvalidSize";
        static constexpr char InvalidSizeType[] = "LuaArrayFormatLoader.InvalidSizeType";
    };

    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<ArrayFormat>>;
    auto loadSize(const sol::object& object) -> std::optional<std::unique_ptr<ArraySizeProvider>>;
    auto loadFixedSize(const sol::object& object)
        -> std::optional<std::unique_ptr<ArraySizeProvider>>;

private:
    void reportInvalidSizeType();
    void reportInvalidSize();
};

} // namespace fuse::binary