#include <fuse/systems/psp/LuaPspLibrary.h>
#include <fuse/systems/psp/TextureSwizzle.h>
#include <sol.hpp>

namespace fuse::psp {

auto openPspLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module["unswizzle"] = &psp::unswizzle;
    module["swizzle"] = &psp::swizzle;
    return module;
}

} // namespace fuse::psp