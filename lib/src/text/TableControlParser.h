#pragma once

#include <kaizo/text/Table.h>
#include <optional>
#include <string>
#include <vector>

namespace kaizo {

class TableControlParser
{
public:
    using argument_t = TableEntry::ParameterFormat::argument_t;

    struct ControlCode
    {
        size_t offset;
        std::string label;
        std::vector<argument_t> arguments;
    };

    auto parse(const std::string& text, size_t index) -> std::optional<ControlCode>;

    void setSource(const std::string& text, size_t index);
    auto parseControl() -> std::optional<ControlCode>;
    auto parseLabel() -> std::optional<std::string>;
    auto parseArguments() -> std::optional<std::vector<argument_t>>;
    auto parseArgument() -> std::optional<argument_t>;
    auto parseHexadecimalArgument() -> std::optional<argument_t>;
    auto parseDecimalArgument() -> std::optional<argument_t>;

private:
    bool hasNext() const;
    bool expectAndConsume(char c);
    auto fetch(size_t offset = 0) -> char;
    bool fetchThenConsume(char c);
    auto fetchAndConsume() -> char;
    void consume(size_t size = 1);

    const std::string* m_text{nullptr};
    size_t m_index{0};
};

} // namespace kaizo