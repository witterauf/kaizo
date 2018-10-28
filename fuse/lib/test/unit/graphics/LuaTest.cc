#include <Catch2.hpp>
#include <fuse/graphics/tiles/TilesLuaLibrary.h>
#include <sol.hpp>
#include <optional>

using namespace fuse::graphics;

SCENARIO("Lua Tiles library", "[Lua]")
{
    sol::state lua;
    lua.require("tiles", sol::c_call<decltype(&openTilesLibrary), &openTilesLibrary>);

    const std::string makeScript =
        "layout = tiles.TileLayout.new{ image_size = { 145, 145 }, tile_counts = { "
        "16, 16 }, borders = { 1, 1, 1, 1 }, grid = { 1, 1 } }";
    const std::string boundingBoxScript = "return layout:bounding_box(1, 1)";

    lua.do_string(makeScript);
    sol::table boundingBox = lua.do_string(boundingBoxScript);

    THEN("A bounding box is returned")
    {
        REQUIRE(boundingBox.size() == 4);

        AND_THEN("The left coordinate should be put into element 1")
        {
            auto maybeLeft = boundingBox.get<sol::optional<unsigned int>>(1);
            REQUIRE(maybeLeft);
            REQUIRE(*maybeLeft == 10);
        }
        AND_THEN("The right coordinate should be put into element 3")
        {
            auto maybeRight = boundingBox.get<sol::optional<unsigned int>>(3);
            REQUIRE(maybeRight);
            REQUIRE(*maybeRight == 18);
        }
        AND_THEN("The top coordinate should be put into element 2")
        {
            auto maybeTop = boundingBox.get<sol::optional<unsigned int>>(2);
            REQUIRE(maybeTop);
            REQUIRE(*maybeTop == 10);
        }
    }
}