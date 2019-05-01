#include <fuse/Binary.h>
#include <fuse/graphics/tiles/Tile.h>
#include <fuse/graphics/tiles/TileColorRemapper.h>
#include <fuse/graphics/tiles/TileFormat.h>
#include <fuse/graphics/tiles/TileImageFormat.h>
#include <fuse/graphics/tiles/TilesLuaLibrary.h>
#include <optional>
#include <sol.hpp>

namespace fuse::graphics {

auto makeTileImageFormat(sol::table table) -> TileImageFormat
{
    if (table.valid())
    {
        auto maybeBorders = table.get<sol::optional<sol::table>>("borders");
        auto maybeGrid = table.get<sol::optional<sol::table>>("grid");
        auto maybeImageSize = table.get<sol::optional<sol::table>>("image_size");
        auto maybeTileCounts = table.get<sol::optional<sol::table>>("tile_counts");
        if (maybeBorders && maybeGrid && maybeImageSize && maybeTileCounts)
        {
            if (maybeBorders->size() == 4 && maybeGrid->size() == 2 &&
                maybeImageSize->size() == 2 && maybeTileCounts->size() == 2)
            {
                TileImageFormatBuilder builder;
                builder.setBorder(
                    maybeBorders->get<unsigned int>(1), maybeBorders->get<unsigned int>(2),
                    maybeBorders->get<unsigned int>(3), maybeBorders->get<unsigned int>(4));
                builder.setGrid(maybeGrid->get<unsigned int>(1), maybeGrid->get<unsigned int>(2));
                builder.setImageSize(maybeImageSize->get<unsigned int>(1),
                                     maybeImageSize->get<unsigned int>(2));
                builder.setTileCount(maybeTileCounts->get<size_t>(1),
                                     maybeTileCounts->get<size_t>(2));
                return builder.build();
            }
        }
    }
    throw std::runtime_error{"error building TileLayout"};
}

auto tileImageFormat_boundingBox(const TileImageFormat& format, unsigned x, unsigned y,
                                 sol::this_state s) -> sol::table
{
    auto const tile = format.tile(x, y);
    sol::state_view lua{s};
    sol::table boundingBox = lua.create_table();
    boundingBox[1] = tile.left();
    boundingBox[2] = tile.top();
    boundingBox[3] = tile.right();
    boundingBox[4] = tile.bottom();
    return boundingBox;
}

auto makeTileFormat(const std::string& formatName, const sol::table& properties)
    -> std::unique_ptr<TileFormat>
{
    std::map<std::string, unsigned> propertyMap;
    for (auto const& element : properties)
    {
        if (element.first.get_type() == sol::type::string)
        {
            if (element.second.get_type() == sol::type::number)
            {
                auto const name = element.first.as<std::string>();
                auto const value = element.second.as<unsigned>();
                propertyMap[name] = value;
            }
        }
    }

    if (auto format = TileFormat::make(formatName, propertyMap))
    {
        return format;
    }
    else
    {
        throw std::runtime_error{"unknown tile format: " + formatName};
    }
}

auto openTilesLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);

    sol::table module = lua.create_table();
    module.new_usertype<Tile>("Tile");
    module.new_usertype<TileImageFormat>("TileLayout", "new", sol::factories(&makeTileImageFormat),
                                         "bounding_box", &tileImageFormat_boundingBox);
    module.new_usertype<TileFormat>("TileFormat", "new", sol::factories(&makeTileFormat), "read",
                                    &TileFormat::read, "write", &TileFormat::write);
    module.new_usertype<TileColorRemapper>(
        "TileColorRemapper", "gray_scaler", sol::factories(&TileColorRemapper::makeGrayScaler),
        "new", sol::constructors<TileColorRemapper()>(), "map", &TileColorRemapper::addMapping,
        "transform", &TileColorRemapper::transform);

    return module;
}

} // namespace fuse::graphics