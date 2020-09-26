#pragma once

#include <stdexcept>
#include <type_traits>

namespace fuse {

template <class T, class U> constexpr T narrow_cast(U&& u) noexcept
{
    return static_cast<T>(std::forward<U>(u));
}

struct narrowing_error : public std::exception
{
};

namespace details {

template <class T, class U>
struct is_same_signedness
    : public std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value>
{
};

} // namespace details

// narrow() : a checked version of narrow_cast() that throws if the cast changed the value
// taken from gsl_util
template <class T, class U> constexpr T narrow(U u) noexcept(false)
{
    constexpr const bool is_different_signedness =
        (std::is_signed<T>::value != std::is_signed<U>::value);

    const T t = narrow_cast<T>(u);

    if (static_cast<U>(t) != u || (is_different_signedness && ((t < T{}) != (u < U{}))))
    {
        throw narrowing_error{};
    }

    return t;
}

} // namespace fuse