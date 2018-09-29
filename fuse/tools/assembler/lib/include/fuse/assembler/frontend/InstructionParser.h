#pragma once

#include "ParserBase.h"
#include <memory>
#include <optional>
#include <vector>

namespace fuse::assembler {

class Instruction;

class InstructionParser : public ParserBase
{
public:
    virtual auto parse(const std::vector<Token>& tokens, size_t index)
        -> ParseResult<std::unique_ptr<Instruction>> = 0;
};

} // namespace fuse::assembler