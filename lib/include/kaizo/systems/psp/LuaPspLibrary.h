#pragma once

#include <sol_forward.hpp>

namespace fuse::psp {

auto openPspLibrary(sol::this_state state) -> sol::table;

}