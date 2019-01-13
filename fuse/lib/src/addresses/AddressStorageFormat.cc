#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressStorageFormat.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/utilities/DomReaderHelpers.h>
#include <fuse/addresses/RelativeStorageFormat.h>

namespace fuse {

auto AddressStorageFormat::deserialize(LuaDomReader& reader)
    -> std::unique_ptr<AddressStorageFormat>
{
    Expects(reader.isRecord());
    auto cls = requireString(reader, "class");
    if (cls == "RelativeOffset")
    {
        return RelativeStorageFormat::deserialize(reader);
    }
    else
    {
        throw FuseException{"'" + cls + "' is not a known address storage class"};
    }
}

} // namespace fuse