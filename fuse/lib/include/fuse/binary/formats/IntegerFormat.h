#pragma once

#include "DataFormat.h"
#include <cstddef>
#include <fuse/Integers.h>

namespace kaizo::data::binary {

class IntegerFormat final : public DataFormat
{
public:
    IntegerFormat() = default;
    explicit IntegerFormat(size_t size, Signedness signedness = Signedness::Unsigned,
                           Endianness endianness = Endianness::Little);

    bool isSigned() const;
    bool isUnsigned() const;
    bool isLittleEndian() const;
    bool isBigEndian() const;
    auto sizeInBits() const -> size_t;
    auto sizeInBytes() const -> size_t;

    auto layout() const -> const IntegerLayout&;

    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    IntegerFormat(const IntegerFormat& other);

    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    IntegerLayout m_layout;
};

} // namespace kaizo::data::binary