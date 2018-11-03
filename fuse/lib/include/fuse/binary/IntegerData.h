#pragma once

#include "Data.h"
#include <variant>

namespace fuse::binary {

class IntegerData : public Data
{
public:
    IntegerData();

    explicit IntegerData(int64_t value);
    explicit IntegerData(uint64_t value);

    bool isNegative() const;
    auto asUnsigned() const -> uint64_t;
    auto asSigned() const -> int64_t;
    auto requiredSize() const -> size_t;
    bool fits(size_t size) const;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    std::variant<int64_t, uint64_t> m_value;
};

} // namespace fuse::binary