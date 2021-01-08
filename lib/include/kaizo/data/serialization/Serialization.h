#pragma once

#include <filesystem>
#include <kaizo/data/data/Data.h>
#include <memory>

namespace kaizo::data {

class DataSerialization
{
public:
    virtual void serialize(const Data& data, const std::filesystem::path& filename) = 0;
    virtual auto deserialize(const std::filesystem::path& filename)
        -> std::unique_ptr<Data> = 0;
};

} // namespace kaizo::data