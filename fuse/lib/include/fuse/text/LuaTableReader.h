#pragma once

#include "Table.h"
#include <filesystem>
#include <optional>
#include <sol_forward.hpp>

namespace fuse::text {

class LuaTableReader
{
public:
    auto load(const std::filesystem::path& luaFile, sol::this_state state) -> std::optional<Table>;
    auto load(const std::filesystem::path& luaFile) -> std::optional<Table>;


};

} // namespace fuse::text