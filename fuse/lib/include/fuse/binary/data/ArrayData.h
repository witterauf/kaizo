#pragma once

#include "Data.h"
#include <vector>

namespace fuse::binary {

class ArrayData : public Data
{
public:
    ArrayData();
    explicit ArrayData(size_t size);

    void append(std::unique_ptr<Data>&& element);
    void set(size_t index, std::unique_ptr<Data>&& element);
    bool hasElement(size_t index) const;
    auto elementType() const -> DataType;
    auto elementCount() const -> size_t;
    auto element(size_t index) const -> const Data&;
    auto element(size_t index) -> Data&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    std::vector<std::unique_ptr<Data>> m_elements;
};

} // namespace fuse::binary