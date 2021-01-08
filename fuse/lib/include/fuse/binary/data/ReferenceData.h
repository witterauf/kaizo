#pragma once

#include "Data.h"
#include <fuse/binary/DataPath.h>

namespace kaizo::data {

class ReferenceData final : public Data
{
public:
    explicit ReferenceData(const DataPath& path);

    auto path() const -> const DataPath&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    ReferenceData(const ReferenceData& other) = default;

    DataPath m_path;
};

} // namespace kaizo::data