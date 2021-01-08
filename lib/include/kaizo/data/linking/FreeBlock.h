#pragma once

#include <cstddef>
#include <kaizo/addresses/Address.h>
#include <string>

namespace kaizo::data {

class SplitFreeBlocks;

class FreeBlock
{
public:
    FreeBlock() = default;
    explicit FreeBlock(const size_t offset, const Address address, size_t size);

    bool isValid() const;
    auto offset() const -> size_t;
    auto address() const -> Address;
    auto size() const -> size_t;
    bool contains(const Address address) const;
    bool fits(size_t size) const;
    bool fits(const Address address, size_t length) const;
    auto allocate(const Address address, size_t length) const -> SplitFreeBlocks;
    auto toString() const -> std::string;
    auto endAddress() const -> Address;

private:
    size_t m_offset{0};
    Address m_address;
    size_t m_size{0};
};

class SplitFreeBlocks
{
public:
    SplitFreeBlocks() = default;
    explicit SplitFreeBlocks(FreeBlock);
    explicit SplitFreeBlocks(FreeBlock, FreeBlock);

    auto size() const -> size_t;
    auto operator[](size_t index) const -> const FreeBlock&;

private:
    FreeBlock m_first;
    FreeBlock m_second;
};

} // namespace kaizo::data