#include <fuse/assembler/ir/UnresolvedReference.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

UnresolvedReference::UnresolvedReference(const std::string& identifier)
    : Expression{ExpressionKind::UnresolvedReference}
    , m_identifier{identifier}
{
    Expects(!identifier.empty());
}

auto UnresolvedReference::identifier() const -> const std::string&
{
    return m_identifier;
}

bool UnresolvedReference::isEqual(const Expression& rhs) const
{
    if (rhs.kind() == ExpressionKind::UnresolvedReference)
    {
        auto const& unresolvedReferenceRhs = static_cast<const UnresolvedReference&>(rhs);
        return unresolvedReferenceRhs.identifier() == identifier();
    }
    return false;
}

} // namespace fuse::assembler