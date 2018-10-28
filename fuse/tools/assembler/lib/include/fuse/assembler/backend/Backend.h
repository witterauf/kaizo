#pragma once

#include "Object.h"
#include <map>
#include <vector>

namespace fuse::assembler {

class AbstractSyntaxTree;
class Block;
class Label;
class InstructionOperand;

class Backend
{
public:
    auto assemble(const AbstractSyntaxTree& ast) -> Object;

private:
    Object m_currentObject;
};

}