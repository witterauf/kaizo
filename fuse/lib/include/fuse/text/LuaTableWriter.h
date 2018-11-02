#pragma once

#include "Table.h"
#include <filesystem>
#include <string>

namespace fuse::text {

class LuaTableWriter
{
public:
    void save(const std::filesystem::path& filename, const Table& table);

private:
    auto writeTable(const Table& table) -> std::string;
    auto writeBinary(const BinarySequence& binary) -> std::string;
    auto writeTableEntry(const TableEntry& entry) -> std::string;
    auto writeText(const TableEntry& entry) -> std::string;
    auto writeControl(const TableEntry& entry) -> std::string;
    auto writeEnd(const TableEntry& entry) -> std::string;
    auto writeTableSwitch(const TableEntry& entry) -> std::string;
    auto writeLabel(const TableEntry::Label& label) -> std::string;
    auto writeParameter(const TableEntry::ParameterFormat& format) -> std::string;
};

} // namespace fuse::text