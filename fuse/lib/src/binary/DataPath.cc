#include "DataPathParser.h"
#include <contracts/Contracts.h>
#include <fuse/binary/DataPath.h>

namespace kaizo::data {

auto DataPathElement::makeName(const std::string& name) -> DataPathElement
{
    DataPathElement element;
    element.m_kind = Kind::Name;
    element.m_value = name;
    return element;
}

auto DataPathElement::makeIndex(size_t index) -> DataPathElement
{
    Expects(index > 0);
    DataPathElement element;
    element.m_kind = Kind::Index;
    element.m_value = index;
    return element;
}

auto DataPathElement::makeIndexWildcard() -> DataPathElement
{
    DataPathElement element;
    element.m_kind = Kind::IndexWildcard;
    return element;
}

auto DataPathElement::makePointer() -> DataPathElement
{
    DataPathElement element;
    element.m_kind = Kind::Pointer;
    return element;
}

auto DataPathElement::kind() const -> Kind
{
    return m_kind;
}

bool DataPathElement::isName() const
{
    return m_kind == Kind::Name;
}

bool DataPathElement::isIndex() const
{
    return m_kind == Kind::Index;
}

bool DataPathElement::isNameExpression() const
{
    return isName();
}

bool DataPathElement::isIndexExpression() const
{
    return isIndex() || kind() == Kind::IndexWildcard;
}

bool DataPathElement::isStatic() const
{
    return isName() || isIndex();
}

bool DataPathElement::isWildcard() const
{
    return kind() == Kind::IndexWildcard;
}

auto DataPathElement::name() const -> const std::string&
{
    Expects(isName());
    return std::get<std::string>(m_value);
}

auto DataPathElement::index() const -> size_t
{
    Expects(isIndex());
    return std::get<size_t>(m_value);
}

auto DataPathElement::toString() const -> std::string
{
    switch (kind())
    {
    case Kind::Name: return name();
    case Kind::Index: return "[" + std::to_string(index()) + "]";
    case Kind::IndexWildcard: return "[*]";
    case Kind::Pointer: return "*";
    default: InvalidCase(kind());
    }
}

bool DataPathElement::operator<(const DataPathElement& element) const
{
    if (kind() == element.kind())
    {
        switch (kind())
        {
        case Kind::Name: return name() < element.name();
        case Kind::Index: return index() < element.index();
        default: return false;
        }
    }
    else
    {
        return kind() < element.kind();
    }
}

bool DataPathElement::operator==(const DataPathElement& element) const
{
    if (kind() != element.kind())
    {
        return false;
    }
    switch (kind())
    {
    case Kind::Name: return name() == element.name();
    case Kind::Index: return name() == element.name();
    default: return true;
    }
}

auto DataPath::operator/=(const DataPathElement& element) -> DataPath&
{
    /*
    if (element.isIndexExpression() && m_elements.size() == 0)
    {
        throw std::invalid_argument{"index expression cannot be the first element of a DataPath"};
    }
    */
    m_elements.push_back(element);
    return *this;
}

void DataPath::goUp()
{
    Expects(m_elements.size() > 0);
    m_elements.pop_back();
}

auto DataPath::toString() const -> std::string
{
    std::string string;
    for (auto i = 0U; i < m_elements.size(); ++i)
    {
        if (i > 0 && m_elements[i].isName())
        {
            string += ".";
        }
        string += m_elements[i].toString();
    }
    return string;
}

bool DataPath::operator<(const DataPath& rhs) const
{
    return m_elements < rhs.m_elements;
}

bool DataPath::operator==(const DataPath& rhs) const
{
    return m_elements == rhs.m_elements;
}

auto DataPath::parent() const -> DataPath
{
    auto path = *this;
    path.goUp();
    return path;
}

auto DataPath::length() const -> size_t
{
    return m_elements.size();
}

auto DataPath::element(size_t index) const -> const DataPathElement&
{
    Expects(index < length());
    return m_elements[index];
}

void DataPath::clear()
{
    m_elements.clear();
}

auto DataPath::fromString(const std::string string) -> std::optional<DataPath>
{
    DataPathParser parser;
    return parser.parse(string);
}

bool DataPath::isEmpty() const
{
    return m_elements.empty();
}

} // namespace kaizo::data