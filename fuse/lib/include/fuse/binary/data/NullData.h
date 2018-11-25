#pragma once

#include "Data.h"

namespace fuse::binary {

class NullData : public Data
{
public:
    NullData();
    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;
};

} // namespace fuse::binary