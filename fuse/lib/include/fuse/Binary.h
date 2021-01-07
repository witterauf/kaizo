#pragma once

#include <cstdint>
#include <filesystem>
#include <fuse/Integers.h>
#include <vector>

namespace kaizo::data {

class BinaryView;

class Binary
{
public:
    static auto load(const std::filesystem::path& filename) -> Binary;
    static auto fromArray(const uint8_t* data, size_t size) -> Binary;
    static auto from(const BinaryView& view) -> Binary;

    Binary() = default;
    explicit Binary(size_t size);

    void save(const std::filesystem::path& filename) const;

    auto size() const -> size_t;
    auto data(size_t offset = 0) const -> const uint8_t*;
    auto data(size_t offset = 0) -> uint8_t*;

    auto read(size_t offset, size_t length) const -> Binary;
    template <class T> auto readAs(size_t offset, const IntegerLayout& layout) const -> T;

    template <class T> auto readAs(size_t offset, size_t length) const -> T
    {
        T result{0};
        for (auto i = 0U; i < length; ++i)
        {
            result |= m_data[offset + i] << (i * 8);
        }
        return result;
    }

    template <size_t N, class T> auto readLittle(size_t offset) const -> T;

    auto asVector() const -> std::vector<uint8_t>;

    void clear();
    void append(uint8_t value);
    void append(char value);

    template <class T> void append(T value, const IntegerLayout& layout);
    template <class T> void write(size_t offset, T value, const IntegerLayout& layout);

    template <class T> void appendLittle(T value, size_t size);
    template <size_t N, class T> void appendLittle(T value);
    template <size_t N, class T> void writeLittle(size_t offset, T value);
    template <class T> void appendBig(T value, size_t size);
    template <size_t N, class T> void appendBig(T value);
    template <size_t N, class T> void writeBig(size_t offset, T value);

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

template <class T> auto Binary::readAs(size_t offset, const IntegerLayout& layout) const -> T
{
    if (layout.endianness == Endianness::Little)
    {
        return readAs<T>(offset, layout.sizeInBytes);
    }
    else
    {
        return readAs<T>(offset, layout.sizeInBytes);
    }
}

template <size_t N, class T> auto Binary::readLittle(size_t offset) const -> T
{
    T result{0};
    for (auto i = 0U; i < N; ++i)
    {
        result |= m_data[offset + i] << (i * 8);
    }
    return result;
}

template <class T> void Binary::append(T value, const IntegerLayout& layout)
{
    if (layout.endianness == Endianness::Little)
    {
        appendLittle(value, layout.sizeInBytes);
    }
    else
    {
        appendBig(value, layout.sizeInBytes);
    }
}

template <class T> void Binary::appendLittle(T value, size_t size)
{
    for (auto i = 0U; i < size; ++i)
    {
        m_data.push_back(value & 0xFF);
        value >>= 8;
    }
}

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

template <class T> void Binary::appendBig(T value, size_t size)
{
    for (auto i = 0U; i < size; ++i)
    {
        auto const byte = static_cast<uint8_t>((value >> ((size - i - 1) * 8)) & 0xFF);
        m_data.push_back(byte);
    }
}

template <size_t N, class T> void Binary::appendBig(T value)
{
    for (auto i = 0U; i < N; ++i)
    {
        auto const byte = static_cast<uint8_t>((value >> ((N - i - 1) * 8)) & 0xFF);
        m_data.push_back(byte);
    }
}

template <size_t N, class T> void Binary::writeBig(size_t offset, T value)
{
    for (auto i = 0U; i < N; ++i)
    {
        auto const byte = static_cast<uint8_t>((value >> ((N - i - 1) * 8)) & 0xFF);
        m_data[offset + i] = byte;
    }
}

} // namespace kaizo::data