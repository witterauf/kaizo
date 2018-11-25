#include "LuaDataFormatLoader.h"
#include <fuse/binary/formats/DataFormat.h>
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
        }
        else
        {
            return false;
        }
    }
    if (hasField(table, "skip_after"))
    {
        if (auto maybeSkip = readField<size_t>(table, "skip_after"))
        {
            format.setSkipAfter(*maybeSkip);
        }
        else
        {
            return false;
        }
    }
    if (hasField(table, "skip_before"))
    {
        if (auto maybeSkip = readField<size_t>(table, "skip_before"))
        {
            format.setSkipBefore(*maybeSkip);
        }
        else
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse::binary