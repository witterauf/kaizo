#pragma once

#include "BlockElement.h"
#include <string>

namespace fuse::assembler {

class Label : public BlockElement
{
protected:
    Label(BlockElementKind kind);
};

} // namespace fuse::assembler