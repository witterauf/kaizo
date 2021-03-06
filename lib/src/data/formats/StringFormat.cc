#include "FormatHelpers.h"
#include <contracts/Contracts.h>
#include <kaizo/data/DataReader.h>
#include <kaizo/data/DataWriter.h>
#include <kaizo/data/data/StringData.h>
#include <kaizo/data/formats/StringFormat.h>

namespace kaizo::data {

StringFormat::StringFormat(const std::shared_ptr<TextEncoding>& encoding)
    : m_encoding{encoding}
{
}

void StringFormat::setEncoding(const std::shared_ptr<TextEncoding>& encoding)
{
    m_encoding = encoding;
}

void StringFormat::setFixedLength(size_t length)
{
    m_fixedLength = length;
}

auto StringFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(m_encoding);
    size_t newOffset;
    std::string string;
    if (m_fixedLength)
    {
        std::tie(newOffset, string) =
            m_encoding->decode(reader.binary().read(reader.offset(), *m_fixedLength), 0);
        newOffset = reader.offset() + *m_fixedLength;
    }
    else
    {
        std::tie(newOffset, string) = m_encoding->decode(reader.binary(), reader.offset());
    }
    auto data = std::make_unique<StringData>();
    data->setValue(string);
    track(reader, reader.offset(), newOffset - reader.offset());
    reader.setOffset(newOffset);
    return std::move(data);
}

void StringFormat::doEncode(DataWriter& writer, const Data& data)
{
    Expects(m_encoding);
    expectDataType(DataType::String, data, writer.path());
    auto const& stringData = static_cast<const StringData&>(data);

    auto binary = m_encoding->encode(stringData.value());
    if (m_fixedLength)
    {
        if (binary.size() > *m_fixedLength)
        {
            throw std::runtime_error{"encoded string exceeds fixed length"};
        }

        auto const length = binary.size();
        for (size_t i = 0; i < *m_fixedLength - length; ++i)
        {
            binary.append(static_cast<uint8_t>(0));
        }
    }
    writer.binary().append(binary);
}

auto StringFormat::copy() const -> std::unique_ptr<DataFormat>
{
    return std::unique_ptr<StringFormat>{new StringFormat{*this}};
}

} // namespace kaizo::data