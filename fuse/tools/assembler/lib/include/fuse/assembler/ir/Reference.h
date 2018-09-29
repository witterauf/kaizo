#pragma once

#include "Expression.h"
#include <string>

namespace fuse::assembler {

class Reference : public Expression
{
public:
    explicit Reference(const std::string& identifier);

    auto identifier() const -> const std::string&;

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const Expression& override;
    bool isEqual(const Expression& rhs) const override;

private:
    std::string m_identifier;
};

} // namespace fuse::assembler