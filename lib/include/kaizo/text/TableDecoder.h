#pragma once

#include "Table.h"
#include <kaizo/binary/BinaryView.h>
#include <map>
#include <optional>
#include <vector>

namespace kaizo {

class HookHandler
{
public:
    virtual ~HookHandler() = default;
    virtual auto decode(const kaizo::BinaryView& binary, size_t offset)
        -> std::optional<std::pair<size_t, std::string>> = 0;
    virtual auto encode(const std::string& name, const std::string& arguments)
        -> std::optional<Binary> = 0;
    virtual auto copy() const -> std::unique_ptr<HookHandler> = 0;
};

class TableDecoder
{
public:
    TableDecoder() = default;
    explicit TableDecoder(const Table& table);

    auto tableCount() const -> size_t;
    void addTable(Table&& table);
    void addTable(const Table& table);
    bool hasTable(const std::string& name) const;
    void setActiveTable(size_t index);
    void setActiveTable(const std::string& name);
    auto activeTable() const -> const Table&;

    void addHook(const std::string& name, HookHandler* hook);

    void setFixedLength(size_t length);
    void unsetFixedLength();

    auto decode(const BinaryView& binary, size_t offset) -> std::pair<size_t, std::string>;

    auto decodeControl(const TableEntry& control) -> std::string;
    auto decodeText(const TableEntry& text) -> std::string;
    auto decodeEnd(const TableEntry& end) -> std::string;
    auto decodeTableSwitch(const TableEntry& tableSwitch) -> std::string;
    auto decodeArgument(const TableEntry::ParameterFormat& format) -> std::string;
    auto decodeHook(const TableEntry& hook) -> std::string;

private:
    size_t m_activeTable{0};
    std::vector<Table> m_tables;
    std::optional<size_t> m_fixedLength;

    std::map<std::string, HookHandler*> m_hooks;

    auto data() const -> const uint8_t*;
    void advance(size_t size);
    auto remaining() const -> size_t;

    const BinaryView* m_binary{nullptr};
    size_t m_offset{0};
};

} // namespace kaizo