#pragma once

#include "Data.h"
#include <fuse/binary/DataPath.h>

namespace fuse::binary {

class ReferenceData : public Data
{
public:
    explicit ReferenceData(const DataPath& path);

    auto path() const -> const DataPath&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    DataPath m_path;
};

} // namespace fuse::binary