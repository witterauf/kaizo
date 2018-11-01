#include <fuse/LuaBaseLibrary.h>
#include <fuse/Binary.h>
#include <sol.hpp>

namespace fuse {

auto loadBinary(std::string filename) -> Binary
{
    return Binary::load(filename);
}

auto openBaseLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);

    sol::table module = lua.create_table();
    module.new_usertype<Binary>("Binary", "load", sol::factories(&loadBinary));

    return module;
}

} // namespace fuse