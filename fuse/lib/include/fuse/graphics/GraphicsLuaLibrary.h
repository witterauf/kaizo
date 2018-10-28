#pragma once

#include <sol_forward.hpp>

namespace fuse::graphics {

auto openGraphicsLibrary(sol::this_state state) -> sol::table;

}