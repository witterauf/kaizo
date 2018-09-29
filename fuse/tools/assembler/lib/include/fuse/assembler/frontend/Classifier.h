#pragma once

#include <cstddef>
#include <optional>
#include <string>

namespace fuse::assembler {

enum class IdentifierKind
{
    Identifier,
    Mnemonic,
    Register,
    Keyword
};

struct Classification
{
    IdentifierKind kind;
    std::optional<size_t> id;
};

class Classifier
{
public:
    virtual auto classify(const std::string& identifier) const -> Classification = 0;
};

}