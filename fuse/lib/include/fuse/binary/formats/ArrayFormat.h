#pragma once

#include "DataFormat.h"
#include <memory>

namespace fuse::binary {

class ArraySizeProvider
{
public:
    virtual ~ArraySizeProvider() = default;
    virtual auto provideSize(const DataReader& reader) const -> size_t = 0;
    virtual auto copy() const -> std::unique_ptr<ArraySizeProvider> = 0;
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

    auto copy() const -> std::unique_ptr<ArraySizeProvider>
    {
        return std::make_unique<FixedSizeProvider>(m_fixedSize);
    }

private:
    size_t m_fixedSize{0};
};

class ArrayFormat final : public DataFormat
{
public:
    ArrayFormat() = default;

    void setSizeProvider(std::unique_ptr<ArraySizeProvider>&& sizeProvider);
    void setElementFormat(std::unique_ptr<DataFormat>&& format);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    ArrayFormat(const ArrayFormat& other);

    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    std::unique_ptr<ArraySizeProvider> m_sizeProvider;
    std::unique_ptr<DataFormat> m_elementFormat;
};

} // namespace fuse::binary