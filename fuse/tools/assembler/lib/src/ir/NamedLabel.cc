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

} // namespace fuse::assembler