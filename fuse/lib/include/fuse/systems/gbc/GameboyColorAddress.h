#include <fuse/addresses/AddressFormat.h>

namespace fuse::systems::gbc {

class GameboyColorAddressFormat : public AddressFormat
{
public:
    auto applyOffset(const Address& address, int64_t offset) const
        -> Address override;
    auto fromInteger(uint64_t address) const -> std::optional<Address> override;
    auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
};

} // namespace fuse::systems::gbc