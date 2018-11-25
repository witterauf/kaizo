#pragma once

#include "DataFormat.h"
#include <cstddef>
#include <fuse/Integers.h>

namespace fuse::binary {

class IntegerFormat : public DataFormat
{
public:
    IntegerFormat() = default;
    explicit IntegerFormat(size_t size, Signedness signedness = Signedness::Unsigned,
                           Endianness endianness = Endianness::Little);
    explicit IntegerFormat(const IntegerLayout& layout);

    bool isSigned() const;
    bool isUnsigned() const;
    bool isLittleEndian() const;
    bool isBigEndian() const;
    auto sizeInBits() const -> size_t;
    auto sizeInBytes() const -> size_t;

    auto layout() const -> const IntegerLayout&;

    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    IntegerLayout m_layout;
};

} // namespace fuse::binary