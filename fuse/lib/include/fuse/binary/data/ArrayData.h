#pragma once

#include "Data.h"
#include <vector>

namespace fuse::binary {

class ArrayData : public Data
{
public:
    ArrayData();

    void append(std::unique_ptr<Data>&& element);
    auto elementType() const -> DataType;
    auto elementCount() const -> size_t;
    auto element(size_t index) const -> const Data&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    std::vector<std::unique_ptr<Data>> m_elements;
};

} // namespace fuse::binary