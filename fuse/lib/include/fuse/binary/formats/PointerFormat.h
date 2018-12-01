#pragma once

#include <fuse/addresses/Address.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/formats/DataFormat.h>
#include <fuse/binary/formats/IntegerFormat.h>
#include <optional>

namespace fuse::binary {

class PointerFormat : public DataFormat
{
public:
    void useAddressMap(bool on);
    void setNullPointer(const Address& null);
    void setAddressFormat(AddressFormat* format);
    void setPointedFormat(std::unique_ptr<DataFormat>&& format);

    auto addressFormat() const -> const AddressFormat&;
    bool hasNullPointer() const;
    auto nullPointer() const -> const Address&;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

    virtual auto readAddress(DataReader& reader) -> std::optional<Address> = 0;
    virtual void writeAddressPlaceHolder(DataWriter& writer) = 0;

    void copyPointerFormat(PointerFormat& format) const;

private:
    AddressFormat* m_addressFormat{nullptr};
    std::unique_ptr<DataFormat> m_pointedFormat;
    bool m_useAddressMap{false};
    std::optional<Address> m_nullPointer;
};

} // namespace fuse::binary