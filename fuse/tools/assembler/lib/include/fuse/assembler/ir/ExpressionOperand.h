#pragma once

#include "Expression.h"
#include "InstructionOperand.h"
#include <memory>

namespace fuse::assembler {

class ExpressionOperand : public InstructionOperand
{
public:
    auto expression() const -> const Expression&;
    auto expression() -> Expression&;

private:
    std::unique_ptr<Expression> m_expression;
};

} // namespace fuse::assembler