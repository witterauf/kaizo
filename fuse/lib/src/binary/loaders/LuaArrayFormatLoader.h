#pragma once

#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/LuaDataFormatLoader.h>

namespace fuse::binary {

class LuaArrayFormatLoader : public LuaFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char InvalidSize[] = "LuaArrayFormatLoader.InvalidSize";
        static constexpr char InvalidSizeType[] = "LuaArrayFormatLoader.InvalidSizeType";
    };

    explicit LuaArrayFormatLoader(LuaDataFormatLoader* loader);

    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<DataFormat>> override;
    auto loadSize(const sol::object& object) -> std::optional<std::unique_ptr<ArraySizeProvider>>;
    auto loadFixedSize(const sol::object& object)
        -> std::optional<std::unique_ptr<ArraySizeProvider>>;

private:
    void reportInvalidSizeType();
    void reportInvalidSize();
};

} // namespace fuse::binary