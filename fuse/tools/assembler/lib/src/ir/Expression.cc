#include <fuse/assembler/ir/Expression.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

Expression::Expression(ExpressionKind kind)
    : m_kind{kind}
{
}

auto Expression::kind() const -> ExpressionKind
{
    return m_kind;
}

auto Expression::operandCount() const -> size_t
{
    return 0;
}

auto Expression::operand(size_t) const -> const Expression&
{
    Expects(false);
}

bool Expression::isEqual(const Expression& rhs) const
{
    if (operandCount() != rhs.operandCount())
    {
        return false;
    }
    for (auto i = 0U; i < operandCount(); ++i)
    {
        if (!operand(i).isEqual(rhs.operand(i)))
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse::assembler