#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/NamedLabel.h>
#include <fuse/assembler/symbols/Symbol.h>

namespace fuse::assembler {

NamedLabel::NamedLabel(Symbol* symbol)
    : Label{BlockElementKind::NamedLabel}
    , m_symbol{symbol}
{
    Expects(symbol);
}

auto NamedLabel::identifier() const -> const std::string&
{
    return m_symbol->identifier();
}

bool NamedLabel::isEqual(const BlockElement& rhs) const
{
    if (rhs.kind() == BlockElementKind::NamedLabel)
    {
        auto const& namedLabelRhs = static_cast<const NamedLabel&>(rhs);
        return m_symbol == namedLabelRhs.m_symbol;
    }
    return false;
}

} // namespace fuse::assembler