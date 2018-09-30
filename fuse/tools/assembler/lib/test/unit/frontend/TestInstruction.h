#pragma once

#include <cstddef>
#include <fuse/assembler/ir/Instruction.h>

class TestInstruction : public fuse::assembler::Instruction
{
public:
    explicit TestInstruction(size_t mnemonic)
        : mnemonic{mnemonic}
    {
    }

    auto operandCount() const -> size_t override;
    auto operand(size_t index) const -> const fuse::assembler::InstructionOperand& override;

    size_t mnemonic;
};