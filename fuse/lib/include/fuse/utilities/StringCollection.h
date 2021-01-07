#pragma once

#include <map>
#include <string>
#include <vector>

namespace kaizo::data {

class StringCollection
{
public:
    auto insert(const std::string& string) -> size_t;
    auto strings() const -> std::vector<std::string>;

private:
    size_t m_index{0};
    std::map<std::string, size_t> m_strings;
};

} // namespace kaizo::data