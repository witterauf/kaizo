#include "kaizo/text/TableEncoding.h"

namespace kaizo {

void TableEncoding::addTable(const Table& table)
{
    m_encoder.addTable(table);
    m_decoder.addTable(table);
}

void TableEncoding::setFixedLength(size_t length)
{
    m_encoder.setFixedLength(length);
    m_decoder.setFixedLength(length);
}

void TableEncoding::setMissingDecoder(std::unique_ptr<MissingDecoder>&& decoder)
{
    m_missingDecoder = std::move(decoder);
    m_decoder.setMissingDecoder(m_missingDecoder.get());
}

bool TableEncoding::canEncode() const
{
    return true;
}

auto TableEncoding::encode(const std::string& text) -> Binary
{
    return m_encoder.encode(text);
}

bool TableEncoding::canDecode() const
{
    return true;
}

auto TableEncoding::decode(const BinaryView& binary, size_t offset)
    -> std::pair<size_t, std::string>
{
    return m_decoder.decode(binary, offset);
}

auto TableEncoding::copy() const -> std::unique_ptr<TextEncoding>
{
    auto encoding = std::make_unique<TableEncoding>();
    encoding->m_decoder = m_decoder;
    encoding->m_encoder = m_encoder;
    encoding->m_missingDecoder = m_missingDecoder->copy();
    return std::move(encoding);
}

} // namespace kaizo