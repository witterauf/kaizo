#pragma once

#include <fuse/binary/LuaDataFormatLoader.h>

namespace fuse::binary {

class LuaIntegerFormatLoader : public LuaFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char InvalidSize[] = "LuaIntegerFormatLoader.InvalidSize";
    };

    auto load(const sol::table& format) -> std::optional<std::unique_ptr<DataFormat>> override;

private:
    void reportInvalidSize(size_t size);
};

} // namespace fuse::binary