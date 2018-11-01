#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/graphics/tiles/TilesLuaLibrary.h>
#include <fuse/graphics/image/LuaImageLibrary.h>
#include <sol.hpp>

namespace fuse::graphics {

auto openGraphicsLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module["tiles"] = openTilesLibrary(state);
    module["image"] = openImageLibrary(state);
    return module;
}

} // namespace fuse::graphics