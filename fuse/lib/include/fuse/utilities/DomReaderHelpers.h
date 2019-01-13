#pragma once

#include "DomReader.h"

namespace fuse {

template <class Key> auto requireString(DomReader& reader, const Key& key) -> std::string
{
    if (!reader.has(key))
    {
        throw FuseException{"the given field does not exist"};
    }
    reader.enter(key);
    if (!reader.isString())
    {
        throw FuseException{"the given field is not a string"};
    }
    auto string = reader.asString();
    reader.leave();
    return string;
}

template <class Key> auto requireUnsignedInteger(DomReader& reader, const Key& key) -> uint64_t
{
    if (!reader.has(key))
    {
        throw FuseException{"the given field does not exist"};
    }
    reader.enter(key);
    if (!reader.isInteger())
    {
        throw FuseException{"the given field is not an integer"};
    }
    auto integer = reader.asInteger();
    if (integer < 0)
    {
        throw FuseException{"the given field is not unsigned"};
    }
    reader.leave();
    return static_cast<uint64_t>(integer);
}

template <class Key> void enterRecord(DomReader& reader, const Key& key)
{
    if (!reader.has(key))
    {
        throw FuseException{"the given field does not exist"};
    }
    reader.enter(key);
    if (!reader.isRecord())
    {
        throw FuseException{"the given field is not a record"};
    }
}

template <class Key> void enterArray(DomReader& reader, const Key& key)
{
    if (!reader.has(key))
    {
        throw FuseException{"the given field does not exist"};
    }
    reader.enter(key);
    if (!reader.isArray())
    {
        throw FuseException{"the given field is not an array"};
    }
}

} // namespace fuse