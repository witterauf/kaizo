#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace kaizo::data::text {

class ShiftJis
{
public:
    static auto toUtf8(uint16_t sjis) -> std::optional<std::string>;
};

} // namespace kaizo::data::text