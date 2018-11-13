#pragma once

#include "DataFormat.h"

namespace fuse::binary {

class ArraySizeProvider
{
public:
    virtual auto provideSize(const DataReader& reader) const -> size_t = 0;
};

class FixedSizeProvider : public ArraySizeProvider
{
public:
    explicit FixedSizeProvider(size_t size)
        : m_fixedSize{size}
    {
    }

    auto provideSize(const DataReader&) const -> size_t override
    {
        return m_fixedSize;
    }

private:
    size_t m_fixedSize{0};
};

class ArrayFormat : public DataFormat
{
public:
    void setSizeProvider(std::unique_ptr<ArraySizeProvider>&& sizeProvider);
    void setElementFormat(std::unique_ptr<DataFormat>&& format);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    std::unique_ptr<ArraySizeProvider> m_sizeProvider;
    std::unique_ptr<DataFormat> m_elementFormat;
};

} // namespace fuse::binary