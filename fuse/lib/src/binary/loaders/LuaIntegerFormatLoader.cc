#include "LuaIntegerFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/IntegerFormat.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaIntegerFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<IntegerFormat>>
{
    size_t size;
    if (auto maybeSize = requireField<size_t>(format, "size"))
    {
        size = *maybeSize;
    }
    else
    {
        return {};
    }
    if (size == 0)
    {
        reportInvalidSize(size);
        return {};
    }

    std::optional<Signedness> maybeSignedness = Signedness::Unsigned;
    std::optional<Endianness> maybeEndianness = Endianness::Little;
    if (hasField(format, "signedness"))
    {
        maybeSignedness = readField<Signedness>(format, "signedness");
    }
    if (hasField(format, "endianness"))
    {
        maybeEndianness = readField<Endianness>(format, "endianness");
    }
    if (!maybeSignedness || !maybeEndianness)
    {
        return {};
    }

    auto integerFormat = std::make_unique<IntegerFormat>(size, *maybeSignedness, *maybeEndianness);
    if (readDataFormat(format, state, *integerFormat))
    {
        return std::move(integerFormat);
    }
    else
    {
        return {};
    }
}

void LuaIntegerFormatLoader::reportInvalidSize(size_t size)
{
    if (hasReporter())
    {
        reporter()
            .report("'%0' is an invalid integer size")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::InvalidSize)
            .substitute(size);
    }
}

} // namespace fuse::binary