#pragma once

#include "InstructionOperand.h"
#include <cstddef>

namespace fuse::assembler {

class RegisterOperand : public InstructionOperand
{
public:
    auto registerIndex() const -> size_t;
    
    template<class T>
    auto registerIndexAs()-> T
    {
        return static_cast<T>(registerIndex());
    }

private:
    size_t m_registerIndex;
};

} // namespace fuse::assembler