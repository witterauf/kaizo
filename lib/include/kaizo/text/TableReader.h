#pragma once

#include "Table.h"
#include <filesystem>
#include <optional>

namespace diagnostics {
class DiagnosticsReporter;
class SourceReporter;
} // namespace diagnostics

namespace fuse::text {

class TableReader
{
public:
    void setReporter(diagnostics::DiagnosticsReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::DiagnosticsReporter&;

    auto read(const std::filesystem::path& filename) -> std::optional<Table>;
    auto read(const char* source, size_t size) -> std::optional<Table>;

private:
    auto doRead(const char* source, size_t size, diagnostics::SourceReporter* reporter = nullptr)
        -> std::optional<Table>;

    void reportFileNotFound(const std::filesystem::path& filename);
    void reportCouldNotOpenFile(const std::filesystem::path& filename);

    diagnostics::DiagnosticsReporter* m_reporter{nullptr};
};

} // namespace fuse::text