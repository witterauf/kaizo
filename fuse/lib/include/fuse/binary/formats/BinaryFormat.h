#pragma once

#include "DataFormat.h"

namespace fuse::binary {

class BinarySizeProvider
{
public:
    virtual ~BinarySizeProvider() = default;
    virtual auto provideSize(const DataReader& reader) const -> size_t = 0;
};

class FixedSizedProvider : public BinarySizeProvider
{
public:
    explicit FixedSizedProvider(size_t fixedSize)
        : m_fixedSize{fixedSize}
    {
    }

    auto provideSize(const DataReader&) const -> size_t override
    {
        return m_fixedSize;
    }

private:
    size_t m_fixedSize{0};
};

class BinaryFormat : public DataFormat
{
public:
    void setSizeProvider(std::unique_ptr<BinarySizeProvider>&& sizeProvider);

protected:
    BinaryFormat(const BinaryFormat& other);

    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;

private:
    std::unique_ptr<BinarySizeProvider> m_sizeProvider;
};

} // namespace fuse::binary