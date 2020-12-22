#pragma once

#include <fuse/BinaryView.h>
#include <map>
#include <memory>
#include <string>

namespace kaizo {

class Tile;
class TileFormatFactory;

class TileFormat
{
public:
    using offset_t = size_t;

    static auto make(const std::string& name, const std::map<std::string, unsigned>& properties)
        -> std::unique_ptr<TileFormat>;
    static void registerTileFormat(const std::string& name,
                                   std::unique_ptr<TileFormatFactory>&& format);

    virtual ~TileFormat() = default;
    virtual auto requiredSize(size_t count) const -> size_t = 0;
    virtual auto write(fuse::MutableBinaryView& buffer, offset_t offset, const Tile& tile)
        -> offset_t = 0;
    virtual auto read(const fuse::BinaryView& buffer, offset_t offset)
        -> std::pair<Tile, offset_t> = 0;
};

class TileFormatFactory
{
public:
    virtual ~TileFormatFactory() = default;
    virtual auto make(const std::map<std::string, unsigned>& properties) const
        -> std::unique_ptr<TileFormat> = 0;
};

} // namespace kaizo