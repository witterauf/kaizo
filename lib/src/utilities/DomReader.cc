#include <kaizo/utilities/DomReader.h>

namespace kaizo::data {

bool DomReader::has(size_t index) const
{
    return index < size();
}

bool DomReader::isRecord() const
{
    return type() == NodeType::Record;
}

bool DomReader::isArray() const
{
    return type() == NodeType::Array;
}

bool DomReader::isInteger() const
{
    return type() == NodeType::Integer;
}

bool DomReader::isString() const
{
    return type() == NodeType::String;
}

} // namespace kaizo::data