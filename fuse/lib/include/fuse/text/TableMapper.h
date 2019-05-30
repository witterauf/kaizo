#pragma once

#include "Table.h"
#include <functional>

namespace fuse::text {

class TableMapper
{
public:
    struct Mapping
    {
        Table::EntryReference entry;
        std::vector<TableEntry::ParameterFormat::argument_t> arguments;
    };

    using Mapper = std::function<bool(const std::string&, const Mapping&)>;

    TableMapper() = default;
    explicit TableMapper(const Table& table);

    auto tableCount() const -> size_t;
    void addTable(Table&& table);
    void addTable(const Table& table);
    bool hasTable(const std::string& name) const;
    void setActiveTable(size_t index);
    void setActiveTable(const std::string& name);
    auto activeTable() const -> const Table&;

    void setMapper(Mapper mapper);

    void map(const std::string& text);

    auto mapCharacters(size_t begin, size_t end) -> std::vector<Table::EntryReference>;
    bool mapControl();

private:
    void tryMappingCharacters(size_t begin, size_t end);
    bool mapCharacters(size_t begin, size_t end, std::vector<Table::EntryReference>& entries);
    auto findNextControl() const -> std::optional<size_t>;
    auto textLength() const -> size_t;
    bool map(const std::string& text, const Mapping& mapping);

    size_t m_activeTable{0};
    std::vector<Table> m_tables;
    const std::string* m_text{nullptr};
    size_t m_index{0};
    Mapper m_mapper;
};

} // namespace fuse::text