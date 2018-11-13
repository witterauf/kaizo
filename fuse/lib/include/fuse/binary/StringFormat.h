#pragma once

#include "DataFormat.h"
#include <fuse/text/TextEncoding.h>

namespace fuse::binary {

class StringFormat : public DataFormat
{
public:
    void setEncoding(text::TextEncoding* encoding);
    auto copy() const -> std::unique_ptr<DataFormat>;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    text::TextEncoding* m_encoding{nullptr};
};

} // namespace fuse::binary