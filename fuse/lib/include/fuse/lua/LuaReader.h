#pragma once

#include <fuse/utilities/DomReader.h>
#include <sol.hpp>
#include <stack>

namespace fuse {

class LuaDomReader : public DomReader
{
public:
    explicit LuaDomReader(const sol::object& root);

    bool has(const std::string& name) const override;
    auto size() const -> size_t override;
    auto type() const -> NodeType override;
    auto asInteger() const -> long long override;
    auto asString() const -> std::string override;

    void enter(const std::string& name) override;
    void enter(size_t index) override;
    void leave() override;

private:
    auto current() const -> const sol::object&;
    auto tableType() const -> NodeType;
    auto numberType() const -> NodeType;

    std::stack<sol::object> m_stack;
};

} // namespace fuse