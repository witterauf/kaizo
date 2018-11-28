#pragma once

#include "DataFormat.h"
#include <fuse/text/TextEncoding.h>
#include <optional>

namespace fuse::binary {

class StringFormat : public DataFormat
{
public:
    StringFormat() = default;
    explicit StringFormat(std::unique_ptr<text::TextEncoding>&& encoding);

    void setEncoding(std::unique_ptr<text::TextEncoding>&& encoding);
    void setFixedLength(size_t length);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    std::optional<size_t> m_fixedLength;
    std::unique_ptr<text::TextEncoding> m_encoding;
};

} // namespace fuse::binary