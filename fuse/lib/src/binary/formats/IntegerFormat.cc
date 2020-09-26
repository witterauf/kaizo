#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/formats/IntegerFormat.h>

namespace fuse::binary {

IntegerFormat::IntegerFormat(size_t size, Signedness signedness, Endianness endianness)
{
    Expects(size > 0);
    m_layout.sizeInBytes = size;
    m_layout.signedness = signedness;
    m_layout.endianness = endianness;
}

bool IntegerFormat::isSigned() const
{
    return m_layout.signedness == Signedness::Signed;
}

bool IntegerFormat::isUnsigned() const
{
    return m_layout.signedness == Signedness::Unsigned;
}

bool IntegerFormat::isLittleEndian() const
{
    return m_layout.endianness == Endianness::Little;
}

bool IntegerFormat::isBigEndian() const
{
    return m_layout.endianness == Endianness::Big;
}

auto IntegerFormat::sizeInBits() const -> size_t
{
    return m_layout.sizeInBytes * 8;
}

auto IntegerFormat::sizeInBytes() const -> size_t
{
    return m_layout.sizeInBytes;
}

auto IntegerFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(sizeInBytes() <= 8);

    uint64_t value{0};

    if (isLittleEndian())
    {
        for (auto i = 0U; i < sizeInBytes(); ++i)
        {
            value += reader.binary()[reader.offset() + i] << (i * 8);
        }
    }
    else
    {
        for (auto i = 0U; i < sizeInBytes(); ++i)
        {
            value *= 256;
            value += reader.binary()[reader.offset() + i];
        }
    }
    reader.advance(sizeInBytes());

    if (isSigned())
    {
        auto signedValue = static_cast<int64_t>(value);
        signedValue <<= 64 - sizeInBits();
        signedValue >>= 64 - sizeInBits(); // arithmetic right shift
        return std::make_unique<IntegerData>(signedValue);
    }
    else
    {
        return std::make_unique<IntegerData>(value);
    }
}

void IntegerFormat::doEncode(DataWriter& writer, const Data& data)
{
    if (data.type() != DataType::Integer)
    {
        throw std::runtime_error{"type mismatch"};
    }
    auto const& integerData = static_cast<const IntegerData&>(data);

    if (isLittleEndian())
    {
        if (isUnsigned())
        {
            writer.binary().appendLittle(integerData.asUnsigned(), sizeInBytes());
        }
        else if (isSigned())
        {
            writer.binary().appendLittle(integerData.asSigned(), sizeInBytes());
        }
    }
    else
    {
        if (isUnsigned())
        {
            writer.binary().appendBig(integerData.asUnsigned(), sizeInBytes());
        }
        else if (isSigned())
        {
            writer.binary().appendBig(integerData.asSigned(), sizeInBytes());
        }
    }
}

auto IntegerFormat::layout() const -> const IntegerLayout&
{
    return m_layout;
}

IntegerFormat::IntegerFormat(const IntegerFormat& other)
    : DataFormat{other}
    , m_layout{other.m_layout}
{
}

auto IntegerFormat::copy() const -> std::unique_ptr<DataFormat>
{
    return std::unique_ptr<IntegerFormat>{new IntegerFormat{*this}};
}

} // namespace fuse::binary