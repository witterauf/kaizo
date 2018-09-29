#pragma once

#include "Expression.h"
#include <memory>

namespace fuse::assembler {

enum class BinaryOperator
{
    Add,
    Subtract,
    Multiply,
    Divide,
    ShiftLeft,
    ShiftRight,
    ArithmeticShiftRight,
    And,
    Or,
    Xor,
    Not
};

class BinaryOperation : public Expression
{
public:
    explicit BinaryOperation(BinaryOperator op, std::unique_ptr<Expression>&& left,
                             std::unique_ptr<Expression>&& right);

    auto op() const -> BinaryOperator;
    auto left() const -> const Expression&;
    auto right() const -> const Expression&;

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const Expression& override;
    bool isEqual(const Expression& rhs) const override;

private:
    const BinaryOperator m_operator;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
};

} // namespace fuse::assembler