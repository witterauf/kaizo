#pragma once

#include "BlockElement.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace fuse::assembler {

class Block
{
public:
    explicit Block(const std::string& identifier);

    void append(std::unique_ptr<BlockElement>&& element);
    auto elementCount() const -> size_t;
    auto element(size_t index) const -> const BlockElement&;
    auto identifier() const -> const std::string&;

    bool isEqual(const Block& rhs) const;

private:
    const std::string m_identifier;
    std::vector<std::unique_ptr<BlockElement>> m_elements;
};

} // namespace fuse::assembler