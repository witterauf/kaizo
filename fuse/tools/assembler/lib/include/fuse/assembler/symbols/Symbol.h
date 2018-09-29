#pragma once

#include "Type.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace fuse::assembler {

using SymbolIdentifier = std::string;
class Expression;

class Symbol
{
public:
    static auto makeConstant(const SymbolIdentifier& identifier, std::unique_ptr<Type>&& type,
                             std::unique_ptr<Expression>&& value) -> std::unique_ptr<Symbol>;
    static auto makeLabel(const SymbolIdentifier& identifier) -> std::unique_ptr<Symbol>;
    static auto makeExternal(const SymbolIdentifier& identifier) -> std::unique_ptr<Symbol>;

    Symbol(const SymbolIdentifier& identifier);
    ~Symbol();

    auto identifier() const -> const SymbolIdentifier&;
    bool isConstant() const;
    bool isExternal() const;
    bool isLabel() const;

    auto type() const -> const Type&;
    auto value() const -> const Expression&;
    bool hasAddress() const;
    auto address() const -> std::optional<size_t>;

    bool isEqual(const Symbol& rhs) const;

private:
    const SymbolIdentifier m_identifier;
    bool m_isExternal{false};
    bool m_isConstant{false};
    bool m_isLabel{false};
    std::unique_ptr<Type> m_type;
    std::unique_ptr<Expression> m_value;
    std::optional<size_t> m_address;
};

} // namespace fuse::assembler