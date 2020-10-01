#include <fuse/systems/LuaSystemsLibrary.h>
#include <fuse/systems/psp/LuaPspLibrary.h>
#include <sol.hpp>

namespace fuse {

auto openSystemsLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module["psp"] = psp::openPspLibrary(state);
    return module;
}

} // namespace fuse