#pragma once

#include <stdexcept>

namespace fuse {

class FuseException : public std::runtime_error
{
public:
    explicit FuseException(const std::string& what)
        : std::runtime_error{ what }
    {
    }
};

}