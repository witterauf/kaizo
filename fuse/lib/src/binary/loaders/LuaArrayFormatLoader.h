#pragma once

#include <fuse/binary/ArrayFormat.h>
#include <fuse/lua/LuaLoader.h>

namespace fuse::binary {

class LuaArrayFormatLoader : public lua::LuaLoader
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