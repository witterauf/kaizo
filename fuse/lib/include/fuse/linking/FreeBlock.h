#pragma once

#include <cstddef>
#include <fuse/addresses/Address.h>
#include <string>

namespace fuse {

class SplitFreeBlocks;

class FreeBlock
{
public:
    FreeBlock() = default;
    explicit FreeBlock(const Address address, size_t size);

    bool isValid() const;
    auto address() const -> Address;
    auto size() const -> size_t;
    bool contains(const Address address) const;
    bool fits(size_t size) const;
    bool fits(const Address address, size_t length) const;
    auto allocate(const Address address, size_t length) const -> SplitFreeBlocks;
    auto toString() const -> std::string;
    auto endAddress() const -> Address;

private:
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

} // namespace fuse