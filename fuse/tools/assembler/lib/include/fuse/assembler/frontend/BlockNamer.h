#pragma once

#include <string>

namespace fuse::assembler {

class BlockNamer
{
public:
    virtual auto generateName() -> std::string = 0;
};

} // namespace fuse::assembler