#pragma once

#include "Transformation.h"

namespace fuse::assembler {

class BlockElement;
class Expression;
class Instruction;

class SymbolResolver : public Transformation
{
public:
    bool transform(CompilationUnit& unit) override;

    bool transform(BlockElement& element);
    bool transform(Expression& expression);
    bool transform(Instruction& instruction);
    bool resolve(Expression& expression, size_t index);

private:
    CompilationUnit* m_unit{nullptr};
};

} // namespace fuse::assembler