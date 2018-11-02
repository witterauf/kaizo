#pragma once

#include "Table.h"
#include <filesystem>
#include <optional>
#include <sol_forward.hpp>

namespace diagnostics {
class DiagnosticsReporter;
}

namespace fuse::text {

class LuaTableReader
{
public:
    void setReporter(diagnostics::DiagnosticsReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::DiagnosticsReporter&;

    auto load(const std::filesystem::path& luaFile, sol::this_state state) -> std::optional<Table>;
    auto load(const std::filesystem::path& luaFile) -> std::optional<Table>;

    auto read(const sol::table& table) -> std::optional<Table>;
    auto readBinary(const sol::table& binary) -> std::optional<BinarySequence>;
    auto readEntry(const sol::object& entry) -> std::optional<TableEntry>;
    auto readSpecial(const sol::table& control) -> std::optional<TableEntry>;
    auto readControl(const sol::table& control) -> std::optional<TableEntry>;
    auto readTableSwitch(const sol::table& control) -> std::optional<TableEntry>;
    auto readEnd(const sol::table& control) -> std::optional<TableEntry>;
    auto readLabel(const sol::table& special) -> std::optional<TableEntry::Label>;
    auto readParameterFormat(const sol::table& table) -> std::optional<TableEntry::ParameterFormat>;

private:
    bool has(const sol::table& table, const std::string& field) const;

    template <class T, class U>
    auto requireField(const sol::table&, const U& name) -> std::optional<T>;
    template <class T, class U>
    auto readOptionalField(const sol::table&, const U& name) -> std::optional<T>;

    void reportResultNotATable();
    void reportResultInvalid();
    void reportBinaryMustHaveElement();
    void reportEntryHasWrongType();
    void reportFieldMissing(const std::string& field);
    void reportFieldWrongType(const std::string& field);
    void reportFieldMissing(size_t index);
    void reportFieldWrongType(size_t index);

    diagnostics::DiagnosticsReporter* m_reporter{nullptr};
};

} // namespace fuse::text