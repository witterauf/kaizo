#pragma once

#include <string>

namespace fuse {

class DomReader
{
public:
    enum class NodeType
    {
        Record,
        Array,
        String,
        Integer,
        Real
    };

    virtual ~DomReader() = default;

    // node access
    virtual bool has(const std::string& name) const = 0;
    virtual auto size() const -> size_t = 0;
    virtual auto type() const -> NodeType = 0;
    virtual auto asInteger() const -> long long = 0;
    virtual auto asString() const -> std::string = 0;

    // parsing state
    virtual void enter(const std::string& name) = 0;
    virtual void enter(size_t index) = 0;
    virtual void leave() = 0;

    // helper methods
    bool has(size_t index) const;
    bool isRecord() const;
    bool isArray() const;
    bool isInteger() const;
    bool isString() const;
};

} // namespace fuse