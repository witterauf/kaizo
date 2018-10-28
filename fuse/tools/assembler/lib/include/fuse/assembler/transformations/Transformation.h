#pragma once

namespace fuse::assembler {

class CompilationUnit;

class Transformation
{
public:
    virtual bool transform(CompilationUnit& unit) = 0;
};

}