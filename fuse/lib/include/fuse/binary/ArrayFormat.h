#pragma once

#include "DataFormat.h"

namespace fuse::binary {

class ArraySizeProvider
{
public:
    virtual auto provideSize() const -> size_t = 0;
};

class ArrayFormat : public DataFormat
{
protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    std::unique_ptr<ArraySizeProvider> m_sizeProvider;
    std::unique_ptr<DataFormat> m_elementFormat;
};

} // namespace fuse::binary