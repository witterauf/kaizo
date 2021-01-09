#include "kaizo/text/TableEncoding.h"

namespace kaizo {

TableEncoding::TableEncoding()
{
    m_mapper.setMapper([this](const std::string& text, const TableMapper::Mapping& mapping) {
        return this->mapChunk(text, mapping);
    });
}

void TableEncoding::addTable(const Table& table)
{
    // inefficient...
    m_encoder.addTable(table);
    m_decoder.addTable(table);
    m_mapper.addTable(table);
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

auto TableEncoding::makeChunks(const std::string& text) const -> std::vector<Chunk>
{
    m_chunks.clear();
    m_mapper.map(text);
    return std::move(m_chunks);
}

bool TableEncoding::mapChunk(const std::string& text, const TableMapper::Mapping& mapping)
{
    m_chunks.push_back({text, mapping});
    return true;
}

} // namespace kaizo
