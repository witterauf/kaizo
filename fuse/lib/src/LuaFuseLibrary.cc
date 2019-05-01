#include <fuse/LuaBaseLibrary.h>
#include <fuse/LuaFuseLibrary.h>
#include <fuse/addresses/LuaAddressLibrary.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/lua/Utilities.h>
#include <fuse/text/LuaTableLibrary.h>
#include <fuse/systems/LuaSystemsLibrary.h>
// clang-format off
#include <sol.hpp>
#include <lfs.h>
// clang-format on

namespace fuse {

void openFuseLibrary(sol::this_state state)
{
    sol::state_view lua{state};
    merge(lua, openBaseLibrary(state));
    lua["addresses"] = openAddressLibrary(state);
    lua["binary"] = binary::openBinaryLibrary(state);
    lua["graphics"] = graphics::openGraphicsLibrary(state);
    lua["text"] = text::openTextLibrary(state);
    lua["systems"] = openSystemsLibrary(state);
    luaopen_lfs(lua.lua_state());
}

} // namespace fuse