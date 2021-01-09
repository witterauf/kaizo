#pragma once

#include "Table.h"
#include "TableDecoder.h"
#include "TableEncoder.h"
#include <fuse/text/TextEncoding.h>
#include <kaizo/text/TableMapper.h>

namespace kaizo::text {

class TableEncoding : public fuse::text::TextEncoding
{
public:
    TableEncoding();

    void addTable(const Table& table);
    void setFixedLength(size_t length);
    void setMissingDecoder(std::unique_ptr<MissingDecoder>&& decoder);

    bool canEncode() const override;
    auto encode(const std::string& text) -> fuse::Binary override;
    bool canDecode() const override;
    auto decode(const fuse::BinaryView& binary, size_t offset)
        -> std::pair<size_t, std::string> override;
    auto copy() const -> std::unique_ptr<TextEncoding> override;

    struct Chunk
    {
        std::string text;
        TableMapper::Mapping mapping;
    };

    auto makeChunks(const std::string& text) const -> std::vector<Chunk>;

private:
    bool mapChunk(const std::string& text, const TableMapper::Mapping& mapping);

    // TODO: make encoder use mapper, share tables
    TableEncoder m_encoder;
    TableDecoder m_decoder;
    mutable TableMapper m_mapper;
    mutable std::vector<Chunk> m_chunks;
    std::unique_ptr<MissingDecoder> m_missingDecoder;
};

} // namespace kaizo::text