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
};

} // namespace fuse::assembler