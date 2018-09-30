#pragma once

#include "BlockElement.h"
#include <map>
#include <string>
#include <vector>

namespace fuse::assembler {

class Directive : public BlockElement
{
public:
    using Value = int64_t;

    explicit Directive(const std::string& identifier);

    auto identifier() const -> const std::string&;

    void addPositional(const Value& value);
    auto positionalCount() const->size_t;
    auto positional(size_t index) const -> const Value&;

    void addNamed(const std::string& name, const Value& value);
    auto namedCount() const->size_t;
    bool hasNamed(const std::string& name) const;
    auto named(const std::string& name) const -> const Value&;

private:
    const std::string& m_identifier;
    std::vector<Value> m_positional;
    std::map<std::string, Value> m_named;
};

} // namespace fuse::assembler