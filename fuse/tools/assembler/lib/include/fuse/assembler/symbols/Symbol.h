#pragma once

#include "Type.h"
#include <cstdint>
#include <memory>
#include <string>

namespace fuse::assembler {

using SymbolIdentifier = std::string;
class Label;
class Expression;

class Symbol
{
public:
    static auto makeConstant(const SymbolIdentifier& identifier, std::unique_ptr<Type>&& type,
                             std::unique_ptr<Expression>&& value) -> std::unique_ptr<Symbol>;
    static auto makeLabel(const SymbolIdentifier& identifier, const Label* label) -> Symbol;
    static auto makeExternal(const SymbolIdentifier& identifier, std::unique_ptr<Type>&& type)
        -> Symbol;
    static auto makeExternalLabel(const SymbolIdentifier& identifier) -> Symbol;

    Symbol(const SymbolIdentifier& identifier);
    ~Symbol();

    auto identifier() const -> const SymbolIdentifier&;
    bool isConstant() const;
    bool isExternal() const;
    bool isLabel() const;

    auto type() const -> const Type&;
    auto value() const -> const Expression&;
    auto label() const -> const Label&;

    bool isEqual(const Symbol& rhs) const;

private:
    const SymbolIdentifier m_identifier;
    bool m_isExternal{false};
    bool m_isConstant{false};
    bool m_isLabel{false};
    std::unique_ptr<Type> m_type;
    std::unique_ptr<Expression> m_value;
    const Label* m_label{nullptr};
};

} // namespace fuse::assembler