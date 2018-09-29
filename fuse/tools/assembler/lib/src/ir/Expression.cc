#include <fuse/assembler/ir/Expression.h>

namespace fuse::assembler {

Expression::Expression(ExpressionKind kind)
    : m_kind{kind}
{
}

auto Expression::kind() const -> ExpressionKind
{
    return m_kind;
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