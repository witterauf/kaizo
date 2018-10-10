#pragma once

#include <memory>

namespace fuse::assembler {

enum class InstructionOperandKind
{
    Register,
    AddressExpression,
    ValueExpression
};

class Reference;

class InstructionOperand
{
public:
    auto kind() const -> InstructionOperandKind;

    bool isExpression() const;
    bool isAddressExpression() const;
    bool isValueExpression() const;
    bool isRegister() const;

    virtual bool usesRelativeAddressing() const;
    virtual bool isUnresolved() const;
    virtual auto makeReference() const -> std::unique_ptr<Reference>;

private:
    const InstructionOperandKind m_kind;
};

} // namespace fuse::assembler