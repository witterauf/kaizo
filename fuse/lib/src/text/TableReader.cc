#include "TableParser.h"
#include <diagnostics/Contracts.h>
#include <diagnostics/FileReporter.h>
#include <diagnostics/InMemoryReporter.h>
#include <fstream>
#include <fuse/text/TableReader.h>
#include <memory>

using namespace diagnostics;

namespace fuse::text {

void TableReader::setReporter(diagnostics::DiagnosticsReporter* reporter)
{
    m_reporter = reporter;
}

bool TableReader::hasReporter() const
{
    return m_reporter != nullptr;
}

auto TableReader::reporter() -> diagnostics::DiagnosticsReporter&
{
    Expects(m_reporter);
    return *m_reporter;
}

namespace fs = std::filesystem;

auto TableReader::read(const std::filesystem::path& filename) -> std::optional<Table>
{
    if (!fs::exists(filename))
    {
        reportFileNotFound(filename);
        return {};
    }
    std::ifstream input{filename, std::ifstream::binary};
    if (input.good())
    {
        auto const filesize = fs::file_size(filename);
        auto buffer = std::make_unique<char[]>(filesize);
        input.read(buffer.get(), filesize);
        if (hasReporter())
        {
            auto fileReporter = reporter().makeProxyUnique<FileReporter>();
            fileReporter->setFileName(filename);
            auto maybeTable = doRead(buffer.get(), filesize, fileReporter.get());
            reporter().unregisterConsumer(fileReporter.get());
            return maybeTable;
        }
        else
        {
            return doRead(buffer.get(), filesize);
        }
    }
    else
    {
        reportCouldNotOpenFile(filename);
        return {};
    }
}

auto TableReader::read(const char* source, size_t size) -> std::optional<Table>
{
    if (hasReporter())
    {
        auto memoryReporter = reporter().makeProxyUnique<InMemoryReporter>();
        auto maybeTable = doRead(source, size, memoryReporter.get());
        reporter().unregisterConsumer(memoryReporter.get());
        return maybeTable;
    }
    else
    {
        return doRead(source, size);
    }
}

auto TableReader::doRead(const char* source, size_t size, SourceReporter* reporter)
    -> std::optional<Table>
{
    Expects(source);
    Expects(size > 0);
    TableParser parser;
    if (reporter)
    {
        reporter->setSource(reinterpret_cast<const uint8_t*>(source), size);
        parser.setReporter(reporter);
    }
    return parser.read(source, size);
}

//##[ diagnostics ]################################################################################

void TableReader::reportFileNotFound(const std::filesystem::path&)
{
}

void TableReader::reportCouldNotOpenFile(const std::filesystem::path&)
{
}

} // namespace fuse::text