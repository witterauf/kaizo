#pragma once

#include "Transformation.h"

namespace fuse::assembler {

class RelativeAddressingResolver : public Transformation
{
public:
    bool transform(CompilationUnit& unit) override;

private:
};

}