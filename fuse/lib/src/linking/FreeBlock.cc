#include "fuse/linking/FreeBlock.h"
#include <contracts/Contracts.h>

namespace kaizo::data {

FreeBlock::FreeBlock(const size_t offset, const Address address, size_t size)
    : m_offset{offset}
    , m_address{address}
    , m_size{size}
{
    Expects(address.isValid());
    Expects(size > 0);
}

auto FreeBlock::offset() const -> size_t
{
    return m_offset;
}

bool FreeBlock::contains(const Address address) const
{
    return !(address < m_address) && address < endAddress();
}

auto FreeBlock::endAddress() const -> Address
{
    return m_address.applyOffset(m_size);
}

bool FreeBlock::isValid() const
{
    return m_size != 0;
}

auto FreeBlock::address() const -> Address
{
    return m_address;
}

auto FreeBlock::size() const -> size_t
{
    return m_size;
}

bool FreeBlock::fits(size_t size) const
{
    return m_size >= size;
}

bool FreeBlock::fits(const Address address, size_t length) const
{
    return !(address < m_address) && (m_size - address.subtract(m_address)) >= length;
}

auto FreeBlock::allocate(const Address address, size_t length) const -> SplitFreeBlocks
{
    if (length == m_size)
    {
        return SplitFreeBlocks{};
    }
    else if (address == m_address)
    {
        return SplitFreeBlocks{
            FreeBlock{m_offset + length, m_address.applyOffset(length), m_size - length}};
    }
    else if (m_address.applyOffset(m_size) == address.applyOffset(length))
    {
        return SplitFreeBlocks{FreeBlock{m_offset, m_address, m_size - length}};
    }
    else
    {
        FreeBlock left{m_offset, m_address, static_cast<size_t>(address.subtract(m_address))};
        auto const allocatedEndAddress = address.applyOffset(length);
        auto const rightSize = endAddress().subtract(allocatedEndAddress);
        FreeBlock right{m_offset + length, allocatedEndAddress, static_cast<size_t>(rightSize)};
        return SplitFreeBlocks{left, right};
    }
}

auto FreeBlock::toString() const -> std::string
{
    auto string = "(" + m_address.toString();
    string += ", ";
    string += std::to_string(m_size);
    string += ")";
    return string;
}

static bool operator<(const FreeBlock& a, const FreeBlock& b)
{
    return a.address() < b.address();
}

SplitFreeBlocks::SplitFreeBlocks(FreeBlock a)
    : m_first{a}
{
}

SplitFreeBlocks::SplitFreeBlocks(FreeBlock a, FreeBlock b)
    : m_first{a}
    , m_second{b}
{
    Expects(a.isValid() && b.isValid());
    Expects(a < b);
}

auto SplitFreeBlocks::size() const -> size_t
{
    size_t size = 0;
    size += m_first.isValid() ? 1 : 0;
    size += m_second.isValid() ? 1 : 0;
    return size;
}

auto SplitFreeBlocks::operator[](size_t index) const -> const FreeBlock&
{
    Expects(index < size());
    if (index == 0)
    {
        return m_first;
    }
    else
    {
        return m_second;
    }
}

} // namespace kaizo::data