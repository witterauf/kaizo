#pragma once

#include "Expression.h"

namespace fuse::assembler {

class Symbol;

class SymbolReference : public Expression
{
public:
    explicit SymbolReference(const Symbol* symbol);

    auto symbol() const -> const Symbol&;

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const Expression& override;
    bool isEqual(const Expression& rhs) const override;

private:
    const Symbol* m_symbol{nullptr};
};

} // namespace fuse::assembler