#pragma once

#include "Label.h"

namespace fuse::assembler {

class Symbol;

class NamedLabel : public Label
{
public:
    explicit NamedLabel(Symbol* symbol);

    auto identifier() const -> const std::string&;

    bool isEqual(const BlockElement& rhs) const override;

private:
    Symbol* m_symbol{nullptr};
};

} // namespace fuse::assembler
