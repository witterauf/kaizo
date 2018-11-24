#include <fuse/binary/Address.h>

namespace fuse::systems::gbc {

class GameboyColorAddressFormat : public binary::AddressFormat
{
public:
    auto applyOffset(const binary::Address& address, int64_t offset) const
        -> binary::Address override;
    auto fromInteger(uint64_t address) const -> std::optional<binary::Address> override;
    auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, binary::Address>> override;
};

} // namespace fuse::systems::gbc