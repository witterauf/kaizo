#include <fuse/LuaBaseLibrary.h>
#include <fuse/LuaFuseLibrary.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/text/LuaTableLibrary.h>
#include <sol.hpp>
#include <lfs.h>

namespace fuse {

void openFuseLibrary(sol::this_state state)
{
    sol::state_view lua{state};
    lua["base"] = openBaseLibrary(state);
    lua["binary"] = binary::openBinaryLibrary(state);
    lua["graphics"] = graphics::openGraphicsLibrary(state);
    lua["text"] = text::openTextLibrary(state);
    luaopen_lfs(lua.lua_state());
}

} // namespace fuse