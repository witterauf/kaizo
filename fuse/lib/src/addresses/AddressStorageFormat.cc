#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressStorageFormat.h>
#include <fuse/addresses/MipsEmbeddedLayout.h>
#include <fuse/addresses/RelativeStorageFormat.h>
#include <fuse/utilities/DomReaderHelpers.h>

namespace fuse {

/*
auto AddressStorageFormat::deserialize(LuaDomReader& reader)
    -> std::unique_ptr<AddressStorageFormat>
{
    Expects(reader.isRecord());
    auto cls = requireString(reader, "class");
    if (cls == "RelativeOffset")
    {
        return RelativeStorageFormat::deserialize(reader);
    }
    else if (cls == "MipsEmbeddedHiLo")
    {
        return MipsEmbeddedLayout::deserialize(reader);
    }
    else
    {
        throw FuseException{"'" + cls + "' is not a known address storage class"};
    }
}

*/

} // namespace fuse