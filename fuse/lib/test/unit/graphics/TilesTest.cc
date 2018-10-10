#include <fstream>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/tiles/TileColorRemapper.h>
#include <fuse/graphics/tiles/TileFont.h>
#include <fuse/graphics/tiles/TileFormat.h>
#include <fuse/graphics/tiles/TileImageFormat.h>
#include <fuse/graphics/tiles/TileList.h>

using namespace fuse::graphics;

int main(int, char* [])
{
    auto const format = TileImageFormatBuilder{}
                            .setBorder(1, 1, 1, 1)
                            .setGrid(1, 1)
                            .setTileCount(16, 16)
                            .setImageSize(128 + 15 + 2, 128 + 15 + 2)
                            .build();

    if (auto maybeImage = Image::load("c:\\build\\font_8x8.png"))
    {
        auto tiles = TileList::fromImage(*maybeImage, format);
        PlanarTileFormat tileFormat;
        tileFormat.setBitsPerPixel(1);
        tileFormat.setTileSize(8, 8);

        TileColorRemapper remap{{0xff000000, 1}, {0xffffffff, 0}};
        tiles = tiles.transform(remap);
        auto const size = tileFormat.requiredSize(tiles.count()) / 8;
        std::vector<uint8_t> data(size, 0);
        tileFormat.writeList(data, 0, tiles);

        std::ofstream out{"c:\\build\\font_8x8.bin", std::ofstream::binary};
        out.write(reinterpret_cast<const char*>(data.data()), size);

        TileFont font{tiles};
        std::vector<uint8_t> widths;
        for (auto i = 0U; i < font.characterCount(); ++i)
        {
            widths.push_back(static_cast<uint8_t>(font.character(i).width()) + 1);
        }
        widths[' '] = 3;

        out = std::ofstream{"c:\\build\\font_8x8_widths.bin", std::ofstream::binary};
        out.write(reinterpret_cast<const char*>(widths.data()), widths.size());

        return 0;
    }

    return 0;
}