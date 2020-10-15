#pragma once

#include "Binary.h"

namespace fuse {

class BinaryView
{
public:
    BinaryView(const Binary& binary);
    explicit BinaryView(const uint8_t* buffer, const size_t size);

    auto data() const -> const uint8_t*;
    auto size() const -> size_t;

    auto begin() const -> const uint8_t*;
    auto end() const -> const uint8_t*;

    auto operator[](const size_t offset) const -> uint8_t;

private:
    const uint8_t* m_buffer{nullptr};
    size_t m_size{0};
};

template <size_t N, class T, class B> auto readLittle(const B& binary, const size_t offset) -> T
{
    T result{0};
    for (size_t i = 0; i < N; ++i)
    {
        result |= binary[offset + i] << (i * 8);
    }
    return result;
}

} // namespace fuse