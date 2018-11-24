#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/IntegerData.h>
#include <fuse/binary/IntegerFormat.h>

namespace fuse::binary {

IntegerFormat::IntegerFormat(size_t size, Signedness signedness, Endianness endianness)
{
    Expects(size > 0);
    m_size = size;
    m_isSigned = signedness == Signedness::Signed;
    m_isLittleEndian = endianness == Endianness::Little;
}

bool IntegerFormat::isSigned() const
{
    return m_isSigned;
}

bool IntegerFormat::isUnsigned() const
{
    return !m_isSigned;
}

bool IntegerFormat::isLittleEndian() const
{
    return m_isLittleEndian;
}

bool IntegerFormat::isBigEndian() const
{
    return !m_isLittleEndian;
}

auto IntegerFormat::sizeInBits() const -> size_t
{
    return m_size * 8;
}

auto IntegerFormat::sizeInBytes() const -> size_t
{
    return m_size;
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

auto IntegerFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto format = std::make_unique<IntegerFormat>();
    format->m_isSigned = m_isSigned;
    format->m_isLittleEndian = m_isLittleEndian;
    format->m_size = m_size;
    copyDataFormat(*format);
    return std::move(format);
}

} // namespace fuse::binary