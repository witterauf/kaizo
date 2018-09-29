#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/Expression.h>
#include <fuse/assembler/symbols/Symbol.h>

namespace fuse::assembler {

auto Symbol::makeConstant(const SymbolIdentifier& identifier, std::unique_ptr<Type>&& type,
                          std::unique_ptr<Expression>&& value) -> std::unique_ptr<Symbol>
{
    auto symbol = std::make_unique<Symbol>(identifier);
    symbol->m_isConstant = true;
    symbol->m_type = std::move(type);
    symbol->m_value = std::move(value);
    return std::move(symbol);
}

auto Symbol::makeLabel(const SymbolIdentifier& identifier) -> std::unique_ptr<Symbol>
{
    auto symbol = std::make_unique<Symbol>(identifier);
    symbol->m_isLabel = true;
    return std::move(symbol);
}

auto Symbol::makeExternal(const SymbolIdentifier& identifier)->std::unique_ptr<Symbol>
{
    auto symbol = std::make_unique<Symbol>(identifier);
    symbol->m_isExternal = true;
    return std::move(symbol);
}

Symbol::Symbol(const SymbolIdentifier& identifier)
    : m_identifier{identifier}
{
    Expects(!identifier.empty());
}

Symbol::~Symbol() = default;

bool Symbol::isEqual(const Symbol& rhs) const
{
    if (identifier() != rhs.identifier())
    {
        return false;
    }
    if (isConstant() && rhs.isConstant())
    {
        return type().isEqual(rhs.type()) && value().isEqual(rhs.value());
    }
    return false;
}

auto Symbol::identifier() const -> const SymbolIdentifier&
{
    return m_identifier;
}

bool Symbol::isConstant() const
{
    return m_isConstant;
}

bool Symbol::isExternal() const
{
    return m_isExternal;
}

bool Symbol::isLabel() const
{
    return m_isLabel;
}

auto Symbol::type() const -> const Type&
{
    Expects(isConstant());
    Expects(m_type);
    return *m_type;
}

auto Symbol::value() const -> const Expression&
{
    Expects(isConstant());
    Expects(m_value);
    return *m_value;
}

bool Symbol::hasAddress() const
{
    Expects(isLabel());
    return m_address.has_value();
}

auto Symbol::address() const -> std::optional<size_t>
{
    Expects(isLabel());
    return m_address;
}

} // namespace fuse::assembler