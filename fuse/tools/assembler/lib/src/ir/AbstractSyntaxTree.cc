#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/AbstractSyntaxTree.h>

namespace fuse::assembler {

void AbstractSyntaxTree::append(std::unique_ptr<Block>&& block)
{
    Expects(block);
    m_blocks.push_back(std::move(block));
}

auto AbstractSyntaxTree::blockCount() const -> size_t
{
    return m_blocks.size();
}

auto AbstractSyntaxTree::block(size_t index) const -> const Block&
{
    Expects(index < blockCount());
    return *m_blocks[index];
}

} // namespace fuse::assembler