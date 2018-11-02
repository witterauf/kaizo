#pragma once

#include <fuse/text/Table.h>
#include <optional>
#include <string>
#include <vector>

namespace fuse::text {

class TableControlParser
{
public:
    using argument_t = TextSequence::Parameter::argument_t;

    struct ControlCode
    {
        std::string label;
        std::vector<argument_t> arguments;
    };

    auto parse(const std::string& text, size_t index) -> std::optional<ControlCode>;

    void setSource(const std::string& text, size_t index);
    auto parseControl() -> std::optional<ControlCode>;
    auto parseLabel() -> std::optional<std::string>;
    auto parseArguments() -> std::optional<std::vector<long>>;
    auto parseArgument() -> std::optional<long>;
    auto parseHexadecimalArgument() -> std::optional<long>;
    auto parseDecimalArgument() -> std::optional<long>;

private:
    bool hasNext() const;
    bool expectAndConsume(char c);
    auto fetch(size_t offset = 0) -> char;
    auto fetchThenConsume(char c) -> char;
    auto fetchAndConsume() -> char;
    void consume(size_t size = 1);

    const std::string* m_text{nullptr};
    size_t m_index{0};
};

} // namespace fuse::text