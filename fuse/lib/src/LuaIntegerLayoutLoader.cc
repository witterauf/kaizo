#include "LuaIntegerLayoutLoader.h"
#include <fuse/lua/Utilities.h>
#include <optional>
#include <sol.hpp>

namespace fuse {

auto loadIntegerLayout(const sol::table& table) -> IntegerLayout
{
    IntegerLayout layout;
    layout.sizeInBytes = requireField<size_t>(table, "size");
    if (hasField(table, "signedness"))
    {
        layout.signedness = readField<Signedness>(table, "signedness");
    }
    else
    {
        layout.signedness = Signedness::Unsigned;
    }
    if (hasField(table, "endianness"))
    {
        layout.endianness = readField<Endianness>(table, "endianness");
    }
    else
    {
        layout.endianness = Endianness::Little;
    }
    if (layout.sizeInBytes == 0)
    {
        throw FuseException{ "integer must have at least size 1" };
    }
    return layout;
}

} // namespace fuse