#pragma once

#include "DataFormat.h"
#include <fuse/text/TableDecoder.h>

namespace fuse::binary {

class StringFormat : public DataFormat
{
public:
private:
};

class TableStringFormat : public DataFormat
{
public:
    void addTable(text::Table&& table);

    auto copy() const -> std::unique_ptr<DataFormat>;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    text::TableDecoder m_decoder;
};

} // namespace fuse::binary