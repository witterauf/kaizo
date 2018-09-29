#pragma once

#include "Block.h"
#include <memory>
#include <vector>

namespace fuse::assembler {

class AbstractSyntaxTree
{
public:
    void append(std::unique_ptr<Block>&& block);
    auto blockCount() const -> size_t;
    auto block(size_t index) const -> const Block&;

private:
    std::vector<std::unique_ptr<Block>> m_blocks;
};

} // namespace fuse::assembler