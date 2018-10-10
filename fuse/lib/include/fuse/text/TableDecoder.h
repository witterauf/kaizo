#pragma once

#include "Table.h"
#include <fuse/Binary.h>
#include <optional>
#include <vector>

namespace fuse::text {

class TableDecoder
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

    auto decode(const Binary& binary, size_t offset) -> std::pair<size_t, std::string>;

    auto decodeControl(const TextSequence& control) -> std::string;
    auto decodeText(const TextSequence& text) -> std::string;
    auto decodeEnd(const TextSequence& end) -> std::string;
    auto decodeTableSwitch(const TextSequence& tableSwitch) -> std::string;

private:
    size_t m_activeTable{0};
    std::vector<Table> m_tables;
    std::optional<size_t> m_fixedLength;
};

} // namespace fuse::text