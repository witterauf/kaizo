#pragma once

#include <fuse/Integers.h>
#include <sol_forward.hpp>

namespace fuse {

auto loadIntegerLayout(const sol::table& table) -> IntegerLayout;

}