#pragma once

#include <fuse/FuseException.h>

namespace fuse {

class FuseLuaException : public FuseException
{
public:
    explicit FuseLuaException(const std::string& what)
        : FuseException{what}
    {
    }
};

} // namespace fuse