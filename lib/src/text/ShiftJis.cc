#include <kaizo/text/ShiftJis.h>
// clang-format off
#include <cstdint>
#include "ShiftJisToUnicode.h"
// clang-format on

namespace kaizo {

static auto toUnicode(uint16_t sjis) -> std::optional<uint32_t>
{
    if (sjis < 0x100)
    {
        return ShiftJis1Byte[sjis];
    }
    else if (sjis >= 0x8140 && sjis < 0x9FFC)
    {
        return ShiftJis2Byte_81[sjis - 0x8140];
    }
    else if (sjis >= 0xE040 && sjis < 0xFCF4)
    {
        return ShiftJis2Byte_E0[sjis - 0xE040];
    }
    else
    {
        return {};
    }
}

auto ShiftJis::toUtf8(uint16_t sjis) -> std::optional<std::string>
{
    if (auto maybeUnicode = toUnicode(sjis))
    {
        auto const unicode = *maybeUnicode;
        std::string result;
        if (unicode < 0x80)
        {
            result += static_cast<char>(unicode);
        }
        else if (unicode < 0x800)
        {
            result += static_cast<char>(0xC0 | (unicode >> 6));
            result += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode < 0x10000)
        {
            result += static_cast<char>(0xE0 | (unicode >> 12));
            result += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode < 0x110000)
        {
            result += static_cast<char>(0xF0 | (unicode >> 18));
            result += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else
        {
            return {};
        }
        return result;
    }
    else
    {
        return {};
    }
}

} // namespace kaizo