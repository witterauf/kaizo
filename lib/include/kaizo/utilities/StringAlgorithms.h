#pragma once

#include <cstddef>
#include <string>

namespace kaizo::data {

template <class T> static auto toString(T value, unsigned base, size_t digits = 0) -> std::string
{
    if (value == 0)
    {
        if (digits != 0)
        {
            return std::string(digits, '0');
        }
        else
        {
            return "0";
        }
    }
    static constexpr char Digits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::string result;
    while (value > 0)
    {
        result += Digits[value % base];
        value /= static_cast<T>(base);
    }
    auto const reversed = std::string{result.rbegin(), result.rend()};
    if (digits != 0)
    {
        if (digits > reversed.size())
        {
            const std::string zeroes(digits - reversed.size(), '0');
            return zeroes + reversed;
        }
        else if (digits < reversed.size())
        {
            return reversed.substr(reversed.size() - digits);
        }
    }
    return reversed;
}

} // namespace kaizo::data