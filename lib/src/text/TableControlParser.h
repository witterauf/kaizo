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

    explicit TableControlParser(const Table* table);

    struct Arguments
    {
        static auto hook(std::string &&) -> Arguments;
        static auto control(std::vector<argument_t> &&) -> Arguments;

        std::string hookArgument;
        std::vector<argument_t> controlArguments;
    };

    struct ControlCode
    {
        size_t offset;
        Table::EntryReference entry;
        Arguments arguments;
    };

    auto parse(const std::string& text, size_t index) -> std::optional<ControlCode>;

    void setSource(const std::string& text, size_t index);
    auto parseControl() -> std::optional<ControlCode>;
    auto parseLabel() -> std::optional<std::string>;

    auto parseArguments(const TableEntry& control) -> std::optional<Arguments>;
    auto parseArguments() -> std::optional<Arguments>;
    auto parseHookArgument() -> std::optional<Arguments>;
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

    const Table* m_table{nullptr};
    const std::string* m_text{nullptr};
    size_t m_index{0};
};

} // namespace kaizo