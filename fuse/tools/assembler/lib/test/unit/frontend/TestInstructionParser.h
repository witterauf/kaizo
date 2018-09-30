#pragma once

#include <fuse/assembler/frontend/InstructionParser.h>

class TestInstructionParser : public fuse::assembler::InstructionParser
{
public:
    virtual auto parse(const std::vector<fuse::assembler::Token>& tokens, size_t index)
        -> fuse::assembler::ParseResult<std::unique_ptr<fuse::assembler::Instruction>> override;
};