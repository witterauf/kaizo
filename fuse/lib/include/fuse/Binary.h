#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace fuse {

class Binary
{
public:
    static auto load(const std::filesystem::path& filename) -> Binary;

    auto size() const -> size_t;
    auto data(size_t offset = 0) const -> const uint8_t*;
    auto read(size_t offset, size_t length) const -> Binary;

    auto begin() const -> const uint8_t*;
    auto end() const -> const uint8_t*;

    auto operator[](size_t offset) const -> uint8_t;

    bool operator==(const Binary& rhs) const;

private:
    std::vector<uint8_t> m_data;
};

} // namespace fuse