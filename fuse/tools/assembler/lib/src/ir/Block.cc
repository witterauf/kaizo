#include <fuse/assembler/ir/Block.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

void Block::append(std::unique_ptr<BlockElement>&& element)
{
    Expects(element);
    m_elements.push_back(std::move(element));
}

auto Block::elementCount() const -> size_t
{
    return m_elements.size();
}

auto Block::element(size_t index) -> const BlockElement&
{
    Expects(index < elementCount());
    return *m_elements[index];
}

} // namespace fuse::assembler