#include "TestInstructionParser.h"
#include "TestInstruction.h"
#include <fuse/assembler/ir/Instruction.h>

using namespace fuse::assembler;

auto TestInstructionParser::parse(const std::vector<Token>& tokens, size_t index)
    -> ParseResult<std::unique_ptr<Instruction>>
{
    setSource(&tokens);
    setIndex(index);

    if (!expect(TokenKind::Mnemonic))
    {
        return {};
    }
    auto instruction = std::make_unique<TestInstruction>(fetchAndConsume().mnemonicId());
    if (!expectAndConsume(TokenKind::EndOfInstruction))
    {
        return {};
    }
    return { std::move(instruction), position() };
}
