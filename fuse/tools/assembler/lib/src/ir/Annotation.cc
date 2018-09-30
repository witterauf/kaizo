#include <fuse/assembler/ir/Annotation.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

Annotation::Annotation(const std::string& identifier)
    : m_identifier{identifier}
{
}

auto Annotation::identifier() const -> const std::string&
{
    return m_identifier;
}

void Annotation::addPositional(const Value& value)
{
    m_positional.push_back(value);
}

auto Annotation::positionalCount() const -> size_t
{
    return m_positional.size();
}

auto Annotation::positional(size_t index) const -> const Value&
{
    Expects(index < positionalCount());
    return m_positional[index];
}

void Annotation::addNamed(const std::string& name, const Value& value)
{
    m_named.insert(std::make_pair(name, value));
}

auto Annotation::namedCount() const -> size_t
{
    return m_named.size();
}

bool Annotation::hasNamed(const std::string& name) const
{
    return m_named.find(name) != m_named.cend();
}

auto Annotation::named(const std::string& name) const -> const Value&
{
    Expects(hasNamed(name));
    return m_named.at(name);
}

} // namespace fuse::assembler