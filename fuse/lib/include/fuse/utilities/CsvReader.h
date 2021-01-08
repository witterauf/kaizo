#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace kaizo::data {

class CsvReader
{
public:
    CsvReader() = default;
    explicit CsvReader(const std::filesystem::path& filename);

    void setColumnName(size_t index, const std::string& name);
    bool hasColumnName(size_t index) const;
    auto columnName(size_t index) const -> const std::string&;

    auto nextRow() -> std::optional<std::vector<std::string>>;

private:
    auto nextColumn() -> std::string;
    auto parseQuoted() -> std::string;
    auto parseNotQuoted() -> std::string;

    bool hasNext() const;
    auto fetch() const -> char;
    void consume();

    std::unique_ptr<unsigned char[]> m_buffer;
    size_t m_position{0};
    size_t m_size{0};

    std::vector<std::optional<std::string>> m_columnNames;
};

} // namespace kaizo::data