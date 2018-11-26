#include <fuse/addresses/AddressFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/formats/AbsolutePointerFormat.h>

namespace fuse::binary {

auto AbsolutePointerFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    if (auto maybeResult = addressFormat().read(reader.binary(), reader.offset()))
    {
        auto [newOffset, address] = *maybeResult;
        reader.setOffset(newOffset);
        return address;
    }
    return {};
}

} // namespace fuse::binary