#include "FreeSpace.h"
#include <algorithm>
#include <diagnostics/Contracts.h>
#include <numeric>

namespace fuse {

static bool operator<(const FreeBlock& lhs, const FreeBlock& rhs)
{
    return lhs.address() < rhs.address();
}

FreeSpace::FreeSpace(std::vector<FreeBlock>&& blocks)
    : m_blocks{std::move(blocks)}
{
    std::sort(m_blocks.begin(), m_blocks.end());
}

auto FreeSpace::findBlockThatContains(const Address address) -> std::optional<size_t>
{
    auto lower = m_blocks.begin();
    auto upper = m_blocks.end();
    while (upper > lower)
    {
        auto const step = std::distance(lower, upper) / 2;
        auto middle = lower + step;
        if (middle->contains(address))
        {
            return std::distance(m_blocks.begin(), middle);
        }
        else if (middle->address() < address)
        {
            lower = middle;
        }
        else
        {
            upper = middle;
        }
        if (step == 0)
        {
            break;
        }
    }
    return {};
}

auto FreeSpace::findFirstBlockThatFits(size_t size) const -> std::optional<size_t>
{
    for (auto i = 0U; i < m_blocks.size(); ++i)
    {
        if (m_blocks[i].size() >= size)
        {
            return i;
        }
    }
    return {};
}

bool FreeSpace::hasBlockThatFits(size_t size) const
{
    return findFirstBlockThatFits(size).has_value();
}

auto FreeSpace::findBlocksThatFit(size_t size) const -> std::vector<size_t>
{
    std::vector<size_t> blocks;
    for (auto i = 0U; i < m_blocks.size(); ++i)
    {
        if (m_blocks[i].size() >= size)
        {
            blocks.push_back(i);
        }
    }
    return blocks;
}

auto FreeSpace::capacity() const -> size_t
{
    return std::accumulate(m_blocks.cbegin(), m_blocks.cend(), static_cast<size_t>(0),
                           [](auto sum, auto const& block) { return sum + block.size(); });
}

auto FreeSpace::block(size_t index) const -> const FreeBlock&
{
    return m_blocks[index];
}

void FreeSpace::allocate(size_t hint, const Address address, size_t length)
{
    Expects(m_blocks[hint].fits(address, length));
    auto const splitBlocks = m_blocks[hint].allocate(address, length);
    Split split;
    split.begin = hint;
    split.end = hint + splitBlocks.size();
    split.original = m_blocks[hint];
    m_splits.push(split);

    if (split.isDeletion())
    {
        m_blocks.erase(m_blocks.begin() + hint);
    }
    else
    {
        m_blocks[hint] = splitBlocks[0];
        for (auto i = 1U; i < splitBlocks.size(); ++i)
        {
            m_blocks.insert(m_blocks.begin() + hint + i, splitBlocks[i]);
        }
    }
}

void FreeSpace::allocate(size_t hint, size_t length)
{
    allocate(hint, m_blocks[hint].address(), length);
}

void FreeSpace::allocateRange(const Address address, size_t length)
{
    if (auto maybeBlock = findBlockThatContains(address))
    {
        // TODO: given address range might span several blocks!
        allocate(*maybeBlock, address, length);
    }
}

void FreeSpace::deallocateLast()
{
    Expects(!m_splits.empty());
    auto const& split = m_splits.top();
    if (split.isDeletion())
    {
        m_blocks.insert(m_blocks.begin() + split.begin, split.original);
    }
    else
    {
        m_blocks[split.begin] = split.original;
        if (split.begin + 1 < split.end)
        {
            m_blocks.erase(m_blocks.begin() + split.begin + 1, m_blocks.end() + split.end);
        }
    }
    m_splits.pop();
}

void FreeSpace::addBlock(const FreeBlock& block)
{
    m_blocks.insert(std::upper_bound(m_blocks.begin(), m_blocks.end(), block), block);
}

} // namespace fuse