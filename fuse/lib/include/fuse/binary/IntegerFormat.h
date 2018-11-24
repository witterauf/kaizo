#pragma once

#include "DataFormat.h"
#include "Integers.h"
#include <cstddef>

namespace fuse::binary {

class IntegerFormat : public DataFormat
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

    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    bool m_isSigned{false};
    bool m_isLittleEndian{true};
    size_t m_size{4};
};

} // namespace fuse::binary