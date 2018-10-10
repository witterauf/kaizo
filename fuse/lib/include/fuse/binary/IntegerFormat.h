#pragma once

#include "DataFormat.h"
#include <cstddef>

namespace fuse::binary {

class IntegerFormat : public DataFormat
{
public:
    bool isSigned() const;
    bool isUnsigned() const;
    bool isLittleEndian() const;
    bool isBigEndian() const;
    auto sizeInBits() const -> size_t;
    auto sizeInBytes() const -> size_t;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    bool m_isSigned{false};
    bool m_isLittleEndian{true};
    size_t m_size{8};
};

} // namespace fuse::binary