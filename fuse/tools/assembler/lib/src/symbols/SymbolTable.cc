#include <fuse/assembler/symbols/SymbolTable.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

void SymbolTable::add(std::unique_ptr<Symbol>&& symbol)
{
    Expects(symbol);
    Expects(!has(symbol->identifier()));
    m_symbols.insert(std::make_pair(symbol->identifier(), std::move(symbol)));
}

bool SymbolTable::has(const SymbolIdentifier& identifier) const
{
    return m_symbols.find(identifier) != m_symbols.cend();
}

auto SymbolTable::lookup(const SymbolIdentifier& identifier) const -> const Symbol*
{
    auto const iter = m_symbols.find(identifier);
    if (iter != m_symbols.cend())
    {
        return iter->second.get();
    }
    else
    {
        return nullptr;
    }
}

} // namespace fuse::assembler