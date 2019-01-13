#include <fuse/Binary.h>
#include <fuse/Integers.h>
#include <fuse/LuaBaseLibrary.h>
#include <sol.hpp>

namespace fuse {

auto loadBinary(const std::string& filename) -> Binary
{
    return Binary::load(filename);
}

void saveBinary(const Binary& binary, const std::string& filename)
{
    binary.save(filename);
}

auto openBaseLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);
    sol::table module = lua.create_table();
    module.new_enum("SIGNEDNESS", "SIGNED", Signedness::Signed, "UNSIGNED", Signedness::Unsigned);
    module.new_enum("ENDIANNESS", "LITTLE", Endianness::Little, "BIG", Endianness::Big);
    module.new_usertype<Binary>("Binary", "load", sol::factories(&loadBinary), "save", &saveBinary);
    return module;
}

} // namespace fuse