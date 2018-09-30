#pragma once

#include "BlockElement.h"
#include <cstddef>

namespace fuse::assembler {

class InstructionOperand;

class Instruction : public BlockElement
{
public:
    virtual auto operandCount() const -> size_t = 0;
    virtual auto operand(size_t index) const -> const InstructionOperand& = 0;

protected:
    Instruction()
        : BlockElement{BlockElementKind::Instruction}
    {
    }
};

} // namespace fuse::assembler