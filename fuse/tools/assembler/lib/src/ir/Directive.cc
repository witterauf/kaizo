#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/Directive.h>

namespace fuse::assembler {

Directive::Directive(const std::string& identifier)
    : BlockElement{BlockElementKind::Directive}
    , m_identifier{identifier}
{
    Expects(!identifier.empty());
}

auto Directive::identifier() const -> const std::string&
{
    return m_identifier;
}

void Directive::addPositional(const Value& value)
{
    m_positional.push_back(value);
}

auto Directive::positionalCount() const -> size_t
{
    return m_positional.size();
}

auto Directive::positional(size_t index) const -> const Value&
{
    Expects(index < positionalCount());
    return m_positional[index];
}

void Directive::addNamed(const std::string& name, const Value& value)
{
    m_named.insert(std::make_pair(name, value));
}

auto Directive::namedCount() const -> size_t
{
    return m_named.size();
}

bool Directive::hasNamed(const std::string& name) const
{
    return m_named.find(name) != m_named.cend();
}

auto Directive::named(const std::string& name) const -> const Value&
{
    Expects(hasNamed(name));
    return m_named.at(name);
}

} // namespace fuse::assembler