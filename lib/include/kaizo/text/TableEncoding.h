#pragma once

#include "Table.h"
#include "TableDecoder.h"
#include "TableEncoder.h"
#include <kaizo/text/TableMapper.h>
#include <kaizo/text/TextEncoding.h>
#include <map>

namespace kaizo {

class TableEncoding : public TextEncoding
{
public:
    TableEncoding();

    void addTable(const Table& table);
    void setFixedLength(size_t length);
    void addHook(const std::string& name, std::shared_ptr<HookHandler> handler);

    bool canEncode() const override;
    auto encode(const std::string& text) -> Binary override;
    bool canDecode() const override;
    auto decode(const BinaryView& binary, size_t offset) -> std::pair<size_t, std::string> override;
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
    std::vector<std::shared_ptr<HookHandler>> m_hooks;
};

} // namespace kaizo