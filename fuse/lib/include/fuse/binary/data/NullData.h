#pragma once

#include "Data.h"

namespace kaizo::data::binary {

class NullData final : public Data
{
public:
    NullData();
    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;
};

} // namespace kaizo::data::binary