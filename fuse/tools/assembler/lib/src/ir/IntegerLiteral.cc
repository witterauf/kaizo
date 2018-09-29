#include <fuse/assembler/ir/IntegerLiteral.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

IntegerLiteral::IntegerLiteral(int64_t value)
    : Expression{ExpressionKind::IntegerLiteral}
    , m_value{value}
{
}

auto IntegerLiteral::value() const -> int64_t
{
    return m_value;
}

auto IntegerLiteral::operandCount() const -> size_t
{
    return 0;
}

auto IntegerLiteral::operand(size_t) const -> const Expression&
{
    Expects(false);
}

bool IntegerLiteral::isEqual(const Expression& rhs) const
{
    if (rhs.kind() == ExpressionKind::IntegerLiteral)
    {
        auto const& integerLiteralRhs = static_cast<const IntegerLiteral&>(rhs);
        return value() == integerLiteralRhs.value() && Expression::isEqual(rhs);
    }
    return false;
}

} // namespace fuse::assembler