#pragma once

#include "Data.h"
#include <cstdint>
#include <fuse/Binary.h>
#include <vector>

namespace fuse::binary {

class BinaryData : public Data
{
public:
    BinaryData();

    auto size() const -> size_t;
    void setData(const Binary& binary);
    void setData(Binary&& binary);
    auto data() const -> const Binary&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    Binary m_data;
};

} // namespace fuse::binary