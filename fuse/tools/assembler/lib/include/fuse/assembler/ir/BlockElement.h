#pragma once

namespace fuse::assembler {

enum class BlockElementKind
{
    Instruction,
    NamedLabel,
    AnonymousLabel,
    Directive
};

class BlockElement
{
public:
    auto kind() const -> BlockElementKind;

protected:
    BlockElement(BlockElementKind kind);

private:
    const BlockElementKind m_kind;
};

} // namespace fuse::assembler