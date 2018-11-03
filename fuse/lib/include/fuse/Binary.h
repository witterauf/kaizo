#pragma once

#include <cstdint>
#include <filesystem>
#include <fuse/binary/Integers.h>
#include <vector>

namespace fuse {

class Binary
{
public:
    static auto load(const std::filesystem::path& filename) -> Binary;
    static auto fromArray(const uint8_t* data, size_t size) -> Binary;

    void save(const std::filesystem::path& filename) const;

    auto size() const -> size_t;
    auto data(size_t offset = 0) const -> const uint8_t*;
    auto read(size_t offset, size_t length) const -> Binary;

    template <class T> auto readAs(size_t offset, size_t length) const -> T
    {
        T result{0};
        for (auto i = 0U; i < length; ++i)
        {
            result |= m_data[offset + i] << (i * 8);
        }
        return result;
    }

    auto asVector() const -> std::vector<uint8_t>;

    void clear();
    void append(uint8_t value);
    void append(char value);

    template <size_t N, class T> void appendLittle(T value);
    template <size_t N, class T> void writeLittle(size_t offset, T value);

    template <class InputIterator> void append(InputIterator begin, InputIterator end)
    {
        for (; begin != end; ++begin)
        {
            append(*begin);
        }
    }

    template <class Container> void append(const Container& container)
    {
        for (auto value : container)
        {
            append(value);
        }
    }

    auto begin() const -> const uint8_t*;
    auto end() const -> const uint8_t*;

    auto operator[](size_t offset) const -> uint8_t;
    auto operator[](size_t offset) -> uint8_t&;

    bool operator==(const Binary& rhs) const;

    auto operator+=(const Binary& rhs) -> Binary&;

private:
    std::vector<uint8_t> m_data;
};

auto operator+(Binary lhs, const Binary& rhs) -> Binary;

//##[ implementation ]#############################################################################

template <size_t N, class T> void Binary::appendLittle(T value)
{
    for (auto i = 0U; i < N; ++i)
    {
        m_data.push_back(value & 0xFF);
        value >>= 8;
    }
}

template <size_t N, class T> void Binary::writeLittle(size_t offset, T value)
{
    for (auto i = 0U; i < N; ++i)
    {
        m_data[offset + i] = value & 0xFF;
        value >>= 8;
    }
}

} // namespace fuse