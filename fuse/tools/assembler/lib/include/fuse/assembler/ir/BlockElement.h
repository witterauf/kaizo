#pragma once

namespace fuse::assembler {

enum class BlockElementKind
{
    Instruction,
    NamedLabel,
    AnonymousLabel,
    Directive
};

class Object;
class SymbolTable;

class BlockElement
{
public:
    auto kind() const -> BlockElementKind;

    bool isAnonymousLabel() const;
    bool isInstruction() const;
    bool isDirective() const;
    bool isNamedLabel() const;
    bool isLabel() const;

    virtual void assemble(Object& object);
    virtual auto binarySize() const -> size_t;
    virtual bool isEqual(const BlockElement& element) const;

protected:
    BlockElement(BlockElementKind kind);

private:
    const BlockElementKind m_kind;
};

} // namespace fuse::assembler