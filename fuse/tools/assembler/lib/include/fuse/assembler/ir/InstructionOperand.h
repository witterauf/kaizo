#pragma once

namespace fuse::assembler {

enum class InstructionOperandKind
{
    Register,
    Expression
};

class InstructionOperand
{
public:
    auto kind() const -> InstructionOperandKind;

private:
    const InstructionOperandKind m_kind;
};

} // namespace fuse::assembler