#pragma once

#include "FreeBlock.h"
#include <optional>
#include <stack>
#include <vector>

namespace kaizo::data {

class FreeSpace
{
public:
    FreeSpace() = default;
    explicit FreeSpace(std::vector<FreeBlock>&& blocks);

    auto findBlockThatContains(const Address address) -> std::optional<size_t>;
    auto findFirstBlockThatFits(size_t size) const -> std::optional<size_t>;
    auto findBlocksThatFit(size_t size) const -> std::vector<size_t>;
    auto findBlockWithinRange(const Address& address, size_t length, size_t size)
        -> std::optional<size_t>;
    // auto findBlockWithAttribute();

    auto capacity() const -> size_t;

    bool hasBlockThatFits(size_t size) const;

    void addBlock(const FreeBlock& block);
    auto blockCount() const -> size_t;
    auto block(size_t index) const -> const FreeBlock&;

    /// Allocates all Addresses within the given range.
    /// Addresses not within the free space are ignored.
    void allocateRange(const Address address, size_t length);

    void allocate(size_t hint, const Address address, size_t length);
    void allocate(size_t hint, size_t length);
    void deallocateLast();

private:
    std::vector<FreeBlock> m_blocks;

    struct Split
    {
        bool isDeletion() const
        {
            return begin == end;
        }

        size_t begin{0}, end{0};
        FreeBlock original;
    };

    std::stack<Split> m_splits;
};

} // namespace kaizo::data