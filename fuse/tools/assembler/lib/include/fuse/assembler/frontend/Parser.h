#pragma once

#include "Token.h"
#include <optional>
#include <vector>

namespace diagnostics {
class SourceReporter;
}

namespace fuse::assembler {

class InstructionParser;

class Parser
{
public:
    void setReporter(diagnostics::SourceReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::SourceReporter*;

    void setInstructionParser(const InstructionParser* parser);

    //auto parse(const std::vector<Token>& tokens) -> std::optional<AbstractSyntaxTree>;
};

} // namespace fuse::assembler