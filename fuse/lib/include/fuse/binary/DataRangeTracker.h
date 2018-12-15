#pragma once

#include <cstddef>
#include <fuse/binary/DataPath.h>
#include <optional>

namespace fuse {

class DataRangeConsumer
{
public:
    virtual ~DataRangeConsumer() = default;

    struct Range
    {
        size_t offset;
        size_t size;
    };

    virtual void track(const binary::DataPath& path, const Range& range,
                       const std::optional<std::string>& tag = {}) = 0;
};

} // namespace fuse