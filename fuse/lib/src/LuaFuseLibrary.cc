#include <fuse/LuaBaseLibrary.h>
#include <fuse/LuaFuseLibrary.h>
#include <fuse/addresses/LuaAddressLibrary.h>
#include <fuse/binary/LuaBinaryLibrary.h>
#include <fuse/lua/Utilities.h>
// clang-format off
#include <sol.hpp>
#include <lfs.h>
// clang-format on

namespace fuse {

static constexpr char TableMerge[] = R"lua(table["merge"] = function(table1, table2)
  local table = {}
  for k, v in pairs(table1) do
    table[k] = v
  end
  for k, v in pairs(table2) do
    table[k] = v
  end
  return table
end)lua";

void openFuseLibrary(sol::this_state state)
{
    sol::state_view lua{state};
    merge(lua, openBaseLibrary(state));
    lua["addresses"] = openAddressLibrary(state);
    lua["binary"] = binary::openBinaryLibrary(state);
    luaopen_lfs(lua.lua_state());
    lua.script(TableMerge);
}

} // namespace fuse