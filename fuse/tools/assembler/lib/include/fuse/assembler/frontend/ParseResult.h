#pragma once

#include <optional>

namespace fuse::assembler {

template <class T> class ParseResult
{
public:
    ParseResult() = default;

    ParseResult(T&& result, size_t index)
        : m_result{std::move(result)}
        , m_index{index}
    {
    }

    ParseResult(const T& result, size_t index)
        : m_result{result}
        , m_index{index}
    {
    }

    auto index() const -> size_t
    {
        return m_index;
    }

    operator bool() const
    {
        return m_result.has_value();
    }

    bool has_value() const
    {
        return m_result.has_value();
    }

    auto operator-> () const -> const T*
    {
        return m_result.operator->();
    }

    auto operator-> () -> T*
    {
        return m_result.operator->();
    }

    auto operator*() const& -> const T&
    {
        return *m_result;
    }

    auto operator*() & -> T&
    {
        return *m_result;
    }

    auto operator*() const && -> const T&&
    {
        return *m_result;
    }

    auto operator*() && -> T&&
    {
        return *m_result;
    }

private:
    std::optional<T> m_result;
    size_t m_index = 0;
};

} // namespace fuse::assembler