#pragma once

#include "Expression.h"
#include <string>

namespace fuse::assembler {

class UnresolvedReference : public Expression
{
public:
    explicit UnresolvedReference(const std::string& identifier);

    auto identifier() const -> const std::string&;
    
    bool isEqual(const Expression& rhs) const override;

private:
    const std::string m_identifier;
};

} // namespace fuse::assembler