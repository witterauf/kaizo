#include "kaizo/graphics/TileFormat.h"
#include "kaizo/graphics/Tile.h"

namespace kaizo {

static std::map<std::string, std::unique_ptr<TileFormatFactory>> TileFormats;

auto TileFormat::make(const std::string& name, const std::map<std::string, unsigned>& properties)
    -> std::unique_ptr<TileFormat>
{
    auto const iter = TileFormats.find(name);
    if (iter != TileFormats.cend())
    {
        return iter->second->make(properties);
    }
    else
    {
        return nullptr;
    }
}

void TileFormat::registerTileFormat(const std::string& name,
                                    std::unique_ptr<TileFormatFactory>&& format)
{
    // Thread safety *probably* not necessary.
    TileFormats.insert(std::make_pair(name, std::move(format)));
}

} // namespace kaizo