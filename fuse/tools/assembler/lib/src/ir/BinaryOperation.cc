#include <fuse/assembler/ir/BinaryOperation.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

BinaryOperation::BinaryOperation(BinaryOperator op, std::unique_ptr<Expression>&& left,
                                 std::unique_ptr<Expression>&& right)
    : Expression{ExpressionKind::BinaryOperation}
    , m_operator{op}
    , m_left{std::move(left)}
    , m_right{std::move(right)}
{
}

auto BinaryOperation::op() const -> BinaryOperator
{
    return m_operator;
}

auto BinaryOperation::left() const -> const Expression&
{
    return *m_left;
}

auto BinaryOperation::right() const -> const Expression&
{
    return *m_right;
}

auto BinaryOperation::operandCount() const -> size_t
{
    return 2;
}

auto BinaryOperation::operand(size_t index) const -> const Expression&
{
    Expects(index < 2);
    if (index == 0)
    {
        return left();
    }
    else if (index == 1)
    {
        return right();
    }
    throw std::logic_error{ "is never reached" };
}

bool BinaryOperation::isEqual(const Expression& rhs) const
{
    if (rhs.kind() == ExpressionKind::BinaryOperation)
    {
        auto const& binaryOperationRhs = static_cast<const BinaryOperation&>(rhs);
        return op() == binaryOperationRhs.op() && Expression::isEqual(rhs);
    }
    return false;
}

} // namespace fuse::assembler