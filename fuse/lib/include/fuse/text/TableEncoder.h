#pragma once

#include "Table.h"
#include <fuse/Binary.h>
#include <optional>

namespace fuse::text {

class TableEncoder
{
public:
    TableEncoder() = default;
    explicit TableEncoder(const Table& table);

    auto tableCount() const -> size_t;
    void addTable(Table&& table);
    void addTable(const Table& table);
    bool hasTable(const std::string& name) const;
    void setActiveTable(size_t index);
    void setActiveTable(const std::string& name);
    auto activeTable() const -> const Table&;

    void setFixedLength(size_t length);
    void unsetFixedLength();

    auto encode(const std::string& text) -> Binary;

    auto encodeCharacters(size_t begin, size_t end)
        -> std::optional<std::pair<size_t, BinarySequence>>;
    bool encodeControl();
    bool encodeControl(const std::string& label,
                       const std::vector<TableEntry::ParameterFormat::argument_t>& arguments);

private:
    void tryEncodeCharacters(size_t begin, size_t end);
    auto findNextControl() const -> std::optional<size_t>;
    auto textLength() const -> size_t;

    const std::string* m_text{nullptr};
    size_t m_index{0};
    Binary m_binary;

    size_t m_activeTable{0};
    std::vector<Table> m_tables;
    std::optional<size_t> m_fixedLength;
};

} // namespace fuse::text