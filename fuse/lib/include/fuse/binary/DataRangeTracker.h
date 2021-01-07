#pragma once

#include <cstddef>
#include <fuse/UsageMap.h>
#include <fuse/binary/DataPath.h>
#include <map>
#include <optional>

namespace kaizo::data {

class LuaWriter;

class DataRangeTracker
{
public:
    virtual ~DataRangeTracker() = default;

    struct Range
    {
        size_t offset;
        size_t size;
    };

    virtual void track(const binary::DataPath& path, const Range& range,
                       const std::optional<std::string>& tag = {}) = 0;
    virtual void serialize(LuaWriter& writer) const = 0;
};

class TagOnlyRangeTracker : public DataRangeTracker
{
public:
    void track(const binary::DataPath& path, const Range& range,
               const std::optional<std::string>& tag = {}) override;
    void serialize(LuaWriter& writer) const override;

private:
    size_t m_alignment{4};
    std::map<std::string, UsageMap> m_maps;
};

} // namespace kaizo::data