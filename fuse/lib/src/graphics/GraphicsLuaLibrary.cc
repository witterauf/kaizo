#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/graphics/tiles/TilesLuaLibrary.h>
#include <fuse/graphics/image/LuaImageLibrary.h>
#include <fuse/graphics/font/LuaFontLibrary.h>
#include <sol.hpp>

namespace fuse::graphics {

auto openGraphicsLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module["tiles"] = openTilesLibrary(state);
    module["image"] = openImageLibrary(state);
    module["font"] = openFontLibrary(state);
    return module;
}

} // namespace fuse::graphics