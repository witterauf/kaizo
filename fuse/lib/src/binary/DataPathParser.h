#pragma once

#include <fuse/binary/DataPath.h>
#include <optional>
#include <string>

namespace kaizo::data {

class DataPathParser
{
public:
    auto parse(const std::string& string) -> std::optional<binary::DataPath>;
    auto parseElement() -> std::optional<binary::DataPathElement>;
    auto parseNameElement() -> std::optional<binary::DataPathElement>;
    auto parseIndexElement() -> std::optional<binary::DataPathElement>;

private:
    bool hasNext() const;
    auto fetch() const -> char;
    void consume();
    bool isFirst() const;

    const std::string* m_string{nullptr};
    size_t m_index{0};
};

} // namespace kaizo::data