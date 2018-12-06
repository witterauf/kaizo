#pragma once

#include <filesystem>
#include <fuse/binary/data/Data.h>
#include <memory>

namespace fuse {

class DataSerialization
{
public:
    virtual void serialize(const binary::Data& data, const std::filesystem::path& filename) = 0;
    virtual auto deserialize(const std::filesystem::path& filename)
        -> std::unique_ptr<binary::Data> = 0;
};

} // namespace fuse