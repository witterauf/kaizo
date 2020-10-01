#pragma once

#include "Table.h"
#include "TableDecoder.h"
#include "TableEncoder.h"
#include "TextEncoding.h"

namespace fuse::text {

class TableEncoding : public TextEncoding
{
public:
    void addTable(const Table& table);
    void setFixedLength(size_t length);
    void setMissingDecoder(std::unique_ptr<MissingDecoder>&& decoder);

    bool canEncode() const override;
    auto encode(const std::string& text) -> Binary override;
    bool canDecode() const override;
    auto decode(const Binary& binary, size_t offset) -> std::pair<size_t, std::string> override;
    auto copy() const -> std::unique_ptr<TextEncoding> override;

private:
    TableEncoder m_encoder;
    TableDecoder m_decoder;
    std::unique_ptr<MissingDecoder> m_missingDecoder;
};

} // namespace fuse::text