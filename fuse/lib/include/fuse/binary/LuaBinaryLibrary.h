#pragma once

#include <sol_forward.hpp>

namespace fuse::binary {

auto openBinaryLibrary(sol::this_state state) -> sol::table;

} // namespace fuse::binary