#pragma once

#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressLayout.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/formats/DataFormat.h>
#include <fuse/binary/formats/IntegerFormat.h>
#include <optional>

namespace kaizo::data::binary {

class PointerFormat : public DataFormat
{
public:
    PointerFormat() = default;

    void useAddressMap(bool on);
    void setNullPointer(const Address& null);
    void setAddressFormat(AddressFormat* format);
    void setPointedFormat(std::unique_ptr<DataFormat>&& format);
    void setLayout(std::unique_ptr<AddressLayout>&& layout);

    auto copy() const -> std::unique_ptr<DataFormat> override;

    auto addressFormat() const -> const AddressFormat&;
    bool hasNullPointer() const;
    auto nullPointer() const -> const Address&;

protected:
    PointerFormat(const PointerFormat& other);

    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

    virtual auto readAddress(DataReader& reader) -> std::optional<Address>;
    virtual void writeAddressPlaceHolder(DataWriter& writer);
    virtual auto makeStorageFormat() -> std::shared_ptr<AddressLayout>;

private:
    AddressFormat* m_addressFormat{nullptr};
    std::unique_ptr<DataFormat> m_pointedFormat;
    bool m_useAddressMap{false};
    std::optional<Address> m_nullPointer;
    std::shared_ptr<AddressLayout> m_layout;
};

} // namespace kaizo::data::binary