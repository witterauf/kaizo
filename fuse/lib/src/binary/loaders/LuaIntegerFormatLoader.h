#pragma once

#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/IntegerFormat.h>

namespace fuse::binary {

class LuaIntegerFormatLoader : public LuaDataFormatLoader
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