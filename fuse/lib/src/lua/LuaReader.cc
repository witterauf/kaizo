#include <diagnostics/Contracts.h>
#include <fuse/FuseException.h>
#include <fuse/lua/LuaReader.h>

namespace fuse {

LuaDomReader::LuaDomReader(const sol::object& root)
{
    m_stack.push(root);
}

bool LuaDomReader::has(const std::string& name) const
{
    Expects(type() == NodeType::Record);
    auto table = current().as<sol::table>();
    auto field = table[name];
    return field.valid();
}

auto LuaDomReader::size() const -> size_t
{
    Expects(type() == NodeType::Array);
    auto table = current().as<sol::table>();
    return table.size();
}

auto LuaDomReader::type() const -> NodeType
{
    switch (current().get_type())
    {
    case sol::type::number: return numberType();
    case sol::type::string: return NodeType::String;
    case sol::type::table: return tableType();
    default: throw FuseException{"unsupported node type"};
    }
}

auto LuaDomReader::tableType() const -> NodeType
{
    auto table = current().as<sol::table>();
    if (table.size() > 0)
    {
        return NodeType::Array;
    }
    else
    {
        return NodeType::Record;
    }
}

auto LuaDomReader::numberType() const -> NodeType
{
    return current().is<long long>() ? NodeType::Integer : NodeType::Real;
}

auto LuaDomReader::asInteger() const -> long long
{
    Expects(type() == NodeType::Integer);
    return current().as<long long>();
}

auto LuaDomReader::asString() const -> std::string
{
    Expects(type() == NodeType::String);
    return current().as<std::string>();
}

void LuaDomReader::enter(const std::string& name)
{
    Expects(has(name));
    auto table = current().as<sol::table>();
    m_stack.push(table.get<sol::object>(name));
}

void LuaDomReader::enter(size_t index)
{
    Expects(index < size());
    auto table = current().as<sol::table>();
    m_stack.push(table.get<sol::object>(index + 1));
}

void LuaDomReader::leave()
{
    m_stack.pop();
}

auto LuaDomReader::current() const -> const sol::object&
{
    return m_stack.top();
}

} // namespace fuse