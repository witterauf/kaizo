#include "LuaDataFormatLoader.h"
#include <fuse/binary/DataFormat.h>
#include <sol.hpp>

namespace fuse::binary {

bool LuaDataFormatLoader::readDataFormat(const sol::table& table, sol::this_state,
                                         DataFormat& format)
{
    if (hasField(table, "alignment"))
    {
        if (auto maybeAlignment = readField<size_t>(table, "alignment"))
        {
            format.setAlignment(*maybeAlignment);
            return true;
        }
    }
    return false;
}

} // namespace fuse::binary