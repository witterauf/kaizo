#pragma once

#include "PointerFormat.h"
#include <fuse/addresses/RelativeStorageFormat.h>
#include <memory>

namespace fuse::binary {

class RelativeOffsetFormat : public PointerFormat
{
public:
    void setOffsetFormat(std::unique_ptr<RelativeStorageFormat>&& offsetFormat);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;
    void writeAddressPlaceHolder(DataWriter&) override;
    auto makeStorageFormat() -> std::shared_ptr<AddressStorageFormat> override;

private:
    std::unique_ptr<RelativeStorageFormat> m_offsetFormat;
    std::shared_ptr<AddressStorageFormat> m_referenceFormat;
};

} // namespace fuse::binary