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

bool BlockElement::isInstruction() const
{
    return m_kind == BlockElementKind::Instruction;
}

bool BlockElement::isAnonymousLabel() const
{
    return m_kind == BlockElementKind::AnonymousLabel;
}

bool BlockElement::isDirective() const
{
    return m_kind == BlockElementKind::Directive;
}

bool BlockElement::isNamedLabel() const
{
    return m_kind == BlockElementKind::NamedLabel;
}

bool BlockElement::isLabel() const
{
    return isAnonymousLabel() || isNamedLabel();
}

bool BlockElement::isEqual(const BlockElement&) const
{
    return false;
}

} // namespace fuse::assembler