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

    bool isAnonymousLabel() const;
    bool isInstruction() const;
    bool isDirective() const;
    bool isNamedLabel() const;
    bool isLabel() const;

    virtual bool isEqual(const BlockElement& element) const;

protected:
    BlockElement(BlockElementKind kind);

private:
    const BlockElementKind m_kind;
};

} // namespace fuse::assembler