#pragma once

#include "BlockElement.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace fuse::assembler {

class Block
{
public:
    void append(std::unique_ptr<BlockElement>&& element);
    auto elementCount() const -> size_t;
    auto element(size_t index) -> const BlockElement&;

private:
    std::vector<std::unique_ptr<BlockElement>> m_elements;
};

} // namespace fuse::assembler