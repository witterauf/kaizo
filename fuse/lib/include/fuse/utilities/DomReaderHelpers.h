#pragma once

#include "DomReader.h"
#include <optional>
#include <stdexcept>

namespace kaizo::data {

namespace details {

template <class Key> auto display(Key key) -> std::string
{
    return std::to_string(key);
}
template <> auto display<const std::string&>(const std::string& key)->std::string;
template <> auto display<const char*>(const char* key)->std::string;

} // namespace details

template <class Key> void require(DomReader& reader, const Key& key)
{
    if (!reader.has(key))
    {
        throw std::runtime_error{"field '" + details::display(key) + "' does not exist"};
    }
}

template <class Key> auto requireString(DomReader& reader, const Key& key) -> std::string
{
    require(reader, key);
    reader.enter(key);
    if (!reader.isString())
    {
        throw std::runtime_error{"the given field is not a string"};
    }
    auto string = reader.asString();
    reader.leave();
    return string;
}

template <class Key>
auto readString(DomReader& reader, const Key& key) -> std::optional<std::string>
{
    if (reader.has(key))
    {
        reader.enter(key);
        if (!reader.isString())
        {
            throw std::runtime_error{"the given field is not a string"};
        }
        auto string = reader.asString();
        reader.leave();
        return string;
    }
    return {};
}

template <class Key>
auto readUnsignedInteger(DomReader& reader, const Key& key) -> std::optional<uint64_t>
{
    if (reader.has(key))
    {
        reader.enter(key);
        if (!reader.isInteger())
        {
            throw std::runtime_error{"the given field is not a string"};
        }
        auto integer = reader.asInteger();
        if (integer < 0)
        {
            throw std::runtime_error{"the given field is not unsigned"};
        }
        reader.leave();
        return static_cast<uint64_t>(integer);
    }
    return {};
}

template <class Key> auto requireUnsignedInteger(DomReader& reader, const Key& key) -> uint64_t
{
    require(reader, key);
    reader.enter(key);
    if (!reader.isInteger())
    {
        throw std::runtime_error{"the given field is not an integer"};
    }
    auto integer = reader.asInteger();
    if (integer < 0)
    {
        throw std::runtime_error{"the given field is not unsigned"};
    }
    reader.leave();
    return static_cast<uint64_t>(integer);
}

template <class Key> auto requireSignedInteger(DomReader& reader, const Key& key) -> int64_t
{
    require(reader, key);
    reader.enter(key);
    if (!reader.isInteger())
    {
        throw std::runtime_error{ "the given field is not an integer" };
    }
    auto integer = reader.asInteger();
    reader.leave();
    return static_cast<int64_t>(integer);
}

template <class Key> void enterRecord(DomReader& reader, const Key& key)
{
    require(reader, key);
    reader.enter(key);
    if (!reader.isRecord())
    {
        throw std::runtime_error{"the given field is not a record"};
    }
}

template <class Key> void enterArray(DomReader& reader, const Key& key)
{
    require(reader, key);
    reader.enter(key);
    if (!reader.isArray())
    {
        throw std::runtime_error{"the given field is not an array"};
    }
}

} // namespace kaizo::data