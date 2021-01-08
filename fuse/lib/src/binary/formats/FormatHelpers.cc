#include "FormatHelpers.h"
#include <stdexcept>

namespace kaizo::data {

void expectDataType(DataType type, const Data& data, const DataPath& path)
{
    if (data.type() != type)
    {
        throw std::runtime_error{"type mismatch at '" + path.toString() + "' (expected " +
                                 toString(type) + ", but got " + toString(data.type()) + ")"};
    }
}

} // namespace kaizo::data