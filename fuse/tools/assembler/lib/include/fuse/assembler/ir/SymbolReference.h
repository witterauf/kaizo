#pragma once

#include "Expression.h"
#include <string>

namespace fuse::assembler {

class Symbol;

class Reference : public Expression
{
public:
    explicit Reference(const Symbol* symbol);

    auto symbol() const -> const Symbol&;

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const Expression& override;
    bool isEqual(const Expression& rhs) const override;

private:
    const Symbol* m_symbol{nullptr};
    std::string m_identifier;
};

} // namespace fuse::assembler