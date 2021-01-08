#pragma once

#include <kaizo/data/DataPath.h>
#include <optional>
#include <string>

namespace kaizo::data {

class DataPathParser
{
public:
    auto parse(const std::string& string) -> std::optional<DataPath>;
    auto parseElement() -> std::optional<DataPathElement>;
    auto parseNameElement() -> std::optional<DataPathElement>;
    auto parseIndexElement() -> std::optional<DataPathElement>;

private:
    bool hasNext() const;
    auto fetch() const -> char;
    void consume();
    bool isFirst() const;

    const std::string* m_string{nullptr};
    size_t m_index{0};
};

} // namespace kaizo::data