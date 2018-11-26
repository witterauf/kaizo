#pragma once

#include "PointerFormat.h"

namespace fuse::binary {

class AbsolutePointerFormat : public PointerFormat
{
protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;
    void writeAddressPlaceHolder(DataWriter&) override{};
};

} // namespace fuse::binary