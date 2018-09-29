#pragma once

#include "Expression.h"
#include <cstdint>

namespace fuse::assembler {

class IntegerLiteral : public Expression
{
public:
    explicit IntegerLiteral(int64_t value);

    auto value() const -> int64_t;

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const Expression& override;
    bool isEqual(const Expression& rhs) const override;

private:
    int64_t m_value{0};
};

}