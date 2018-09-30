#include "TestInstruction.h"
#include <diagnostics/Contracts.h>

using namespace fuse::assembler;

auto TestInstruction::operandCount() const -> size_t
{
    return 0;
}

auto TestInstruction::operand(size_t) const -> const InstructionOperand&
{
    Expects(false);
}
