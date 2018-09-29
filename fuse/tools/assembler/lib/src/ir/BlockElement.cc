#include <fuse/assembler/ir/BlockElement.h>

namespace fuse::assembler {

auto BlockElement::kind() const -> BlockElementKind
{
    return m_kind;
}

BlockElement::BlockElement(BlockElementKind kind)
    : m_kind{kind}
{
}

bool BlockElement::isEqual(const BlockElement&) const
{
    return false;
}

} // namespace fuse::assembler