#pragma once

#include "Table.h"
#include <fuse/Binary.h>
#include <optional>

namespace fuse::text {

class TableEncoder
{
public:
    auto tableCount() const -> size_t;
    void addTable(Table&& table);
    bool hasTable(const std::string& name) const;
    void setActiveTable(size_t index);
    void setActiveTable(const std::string& name);
    auto activeTable() const -> const Table&;

    void setFixedLength(size_t length);
    void unsetFixedLength();

    auto encode(const std::string& text) -> Binary;

    void encodeCharacters();
    bool encodeControl();
    bool encodeControl(const std::string& label, const std::vector<long>& arguments);

    auto parseLabel() -> std::optional<std::string>;
    auto parseArguments() -> std::optional<std::vector<long>>;
    auto parseArgument() -> std::optional<long>;

private:
    auto fetch(size_t offset = 0) -> char;
    void consume(size_t size = 1);
    auto textLength() const -> size_t;

    const std::string* m_text{nullptr};
    size_t m_index{0};
    Binary m_binary;

    size_t m_activeTable{0};
    std::vector<Table> m_tables;
    std::optional<size_t> m_fixedLength;
};

} // namespace fuse::text