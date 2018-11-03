#include <diagnostics/Contracts.h>
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/lua/LuaLoader.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::lua {

void LuaLoader::setReporter(diagnostics::DiagnosticsReporter* reporter)
{
    m_reporter = reporter;
}

bool LuaLoader::hasReporter() const
{
    return m_reporter != nullptr;
}

auto LuaLoader::reporter() -> diagnostics::DiagnosticsReporter&
{
    Expects(hasReporter());
    return *m_reporter;
}

void LuaLoader::reportFieldMissing(const std::string& field)
{
    if (hasReporter())
    {
        reporter()
            .report("field '%0' is required but missing")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticsTags::FieldMissing)
            .substitute(field);
    }
}

void LuaLoader::reportFieldWrongType(const std::string& field)
{
    if (hasReporter())
    {
        reporter()
            .report("required field '%0' has the wrong type")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticsTags::FieldWrongType)
            .substitute(field);
    }
}

void LuaLoader::reportFieldMissing(size_t index)
{
    if (hasReporter())
    {
        reporter()
            .report("element '[%0]' is required but missing")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticsTags::FieldMissing)
            .substitute(std::to_string(index));
    }
}

void LuaLoader::reportFieldWrongType(size_t index)
{
    if (hasReporter())
    {
        reporter()
            .report("required element '[%0]' has the wrong type")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticsTags::FieldWrongType)
            .substitute(std::to_string(index));
    }
}

} // namespace fuse::lua