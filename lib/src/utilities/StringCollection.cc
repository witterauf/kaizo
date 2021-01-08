#include <kaizo/utilities/StringCollection.h>

namespace kaizo {

auto StringCollection::insert(const std::string& string) -> size_t
{
    auto const iter = m_strings.find(string);
    if (iter != m_strings.cend())
    {
        return iter->second;
    }
    else
    {
        m_strings.insert(std::make_pair(string, m_index));
        return m_index++;
    }
}

auto StringCollection::strings() const -> std::vector<std::string>
{
    std::vector<std::string> result(m_strings.size());
    for (auto const& stringPair : m_strings)
    {
        result[stringPair.second] = stringPair.first;
    }
    return result;
}

} // namespace kaizo