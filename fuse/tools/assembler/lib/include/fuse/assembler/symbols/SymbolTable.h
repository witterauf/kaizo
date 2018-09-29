#pragma once

#include "Symbol.h"
#include <map>

namespace fuse::assembler {

class SymbolTable
{
public:
    void add(std::unique_ptr<Symbol>&& symbol);
    bool has(const SymbolIdentifier& identifier) const;
    auto lookup(const SymbolIdentifier& identifier) const -> const Symbol*;
    auto lookup(const SymbolIdentifier& identifier) -> Symbol*;

private:
    std::map<SymbolIdentifier, std::unique_ptr<Symbol>> m_symbols;
};

}