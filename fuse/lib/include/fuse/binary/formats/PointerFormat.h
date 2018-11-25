#pragma once

#include <fuse/addresses/Address.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/formats/DataFormat.h>
#include <fuse/binary/formats/IntegerFormat.h>

namespace fuse::binary {

class PointerFormat : public DataFormat
{
public:
    void useAddressMap(bool on);
    void setAddressFormat(std::unique_ptr<AddressFormat>&& format);
    void setPointedFormat(std::unique_ptr<DataFormat>&& format);
    auto addressFormat() const -> const AddressFormat&;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;
    void copyPointerFormat(PointerFormat& format) const;

    struct AddressDescriptor
    {
        static auto ignoreAddress() -> AddressDescriptor
        {
            return AddressDescriptor{};
        }

        AddressDescriptor() = default;
        AddressDescriptor(Address address)
            : address{address}
        {
        }

        std::optional<Address> address;
    };

    virtual auto readAddress(DataReader& reader) -> std::optional<AddressDescriptor> = 0;
    virtual void writeAddressPlaceHolder(DataWriter& writer) = 0;

private:
    std::unique_ptr<AddressFormat> m_addressFormat;
    std::unique_ptr<DataFormat> m_pointedFormat;
    bool m_useAddressMap{false};
};

class AbsolutePointerFormat : public PointerFormat
{
protected:
    auto readAddress(DataReader& reader) -> std::optional<AddressDescriptor> override;
    void writeAddressPlaceHolder(DataWriter&) override{};
};

class BaseAddressProvider
{
public:
    virtual auto provideAddress() const -> Address = 0;
    virtual auto copy() const -> std::unique_ptr<BaseAddressProvider> = 0;
};

class FixedBaseAddressProvider : public BaseAddressProvider
{
public:
    explicit FixedBaseAddressProvider(Address address)
        : m_address{address}
    {
    }

    auto provideAddress() const -> Address override
    {
        return m_address;
    }

    auto copy() const -> std::unique_ptr<BaseAddressProvider> override
    {
        return std::make_unique<FixedBaseAddressProvider>(m_address);
    }

private:
    Address m_address;
};

class OffsetValidator
{
public:
    virtual bool isValid(const IntegerData& offset) const = 0;
    virtual auto copy() const -> std::unique_ptr<OffsetValidator> = 0;
};

class FixedOffsetValidator : public OffsetValidator
{
public:
    explicit FixedOffsetValidator(std::unique_ptr<IntegerData>&& offset)
        : m_offset{std::move(offset)}
    {
    }

    bool isValid(const IntegerData& offset) const override
    {
        return !offset.isEqual(*m_offset);
    }

    auto copy() const -> std::unique_ptr<OffsetValidator> override
    {
        return std::make_unique<FixedOffsetValidator>(m_offset->copyAs<IntegerData>());
    }

private:
    std::unique_ptr<IntegerData> m_offset;
};

class RelativeOffsetFormat : public PointerFormat
{
public:
    void setOffsetValidator(std::unique_ptr<OffsetValidator>&& validator);
    void setBaseAddressProvider(std::unique_ptr<BaseAddressProvider>&& provider);
    void setOffsetFormat(std::unique_ptr<IntegerFormat>&& offsetFormat);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto readAddress(DataReader& reader) -> std::optional<AddressDescriptor> override;
    void writeAddressPlaceHolder(DataWriter&) override{};

private:
    std::unique_ptr<OffsetValidator> m_validator;
    std::unique_ptr<IntegerFormat> m_offsetFormat;
    std::unique_ptr<BaseAddressProvider> m_baseProvider;
};

} // namespace fuse::binary