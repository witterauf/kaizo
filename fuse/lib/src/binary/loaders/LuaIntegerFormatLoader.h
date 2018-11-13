#pragma once

#include <fuse/binary/IntegerFormat.h>
#include <fuse/lua/LuaLoader.h>

namespace fuse::binary {

class LuaIntegerFormatLoader : public lua::LuaLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char InvalidSize[] = "LuaIntegerFormatLoader.InvalidSize";
    };

    auto load(const sol::table& format, sol::this_state)
        -> std::optional<std::unique_ptr<IntegerFormat>>;

private:
    void reportInvalidSize(size_t size);
};

} // namespace fuse::binary