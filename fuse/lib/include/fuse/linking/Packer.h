#pragma once

#include "Target.h"
#include <filesystem>

namespace fuse {

class FreeBlock;
class LinkObject;

class Packer
{
public:
    virtual void addFreeBlock(const FreeBlock& block) = 0;
    virtual void addObject(LinkObject* object) = 0;
    virtual bool pack() = 0;
};

} // namespace fuse