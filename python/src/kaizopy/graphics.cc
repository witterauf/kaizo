#include <filesystem>
#include <iostream>
#include <kaizo/graphics/ImageFileFormat.h>
#include <kaizo/graphics/TileFormat.h>
#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;
using namespace kaizo;

static auto Tile_load(const std::string& filename) -> std::pair<Tile, std::optional<Palette>>
{
    std::filesystem::path path{filename};
    if (!path.has_extension())
    {
        throw py::value_error{"unsupported image file format: '" + path.string() + "'"};
    }

    auto const format = ImageFileFormat::makeFormat(path.extension().string().substr(1));
    if (!format)
    {
        throw py::value_error{"unsupported image file format: '" + path.string() + "'"};
    }

    auto maybeImageWithPalette = format->load(path);
    if (!maybeImageWithPalette)
    {
        throw py::value_error{"could not load image file '" + path.string() + "'"};
    }

    return std::move(*maybeImageWithPalette);
}

static void Tile_save(const Tile& tile, const std::string& filename)
{
    std::filesystem::path path{filename};
    if (!path.has_extension())
    {
        throw py::value_error{"unsupported image file format: '" + path.string() + "'"};
    }

    auto const format = ImageFileFormat::makeFormat(path.extension().string().substr(1));
    if (!format)
    {
        throw py::value_error{"unsupported image file format: '" + path.string() + "'"};
    }

    if (!format->save(path, tile, nullptr))
    {
        throw py::value_error{"could not save to image file '" + path.string() + "'"};
    }
}

static auto PixelFormat_make_channels(const std::string& channels, const size_t bpc) -> PixelFormat
{
    if (channels == "R.G.B.A" && bpc == 8)
    {
        return PixelFormat::rgba();
    }
    else
    {
        throw py::value_error{"unknown channel format"};
    }
}

static auto PixelFormat_kind(const PixelFormat& format) -> std::string
{
    switch (format.format())
    {
    case PixelFormat::Format::Indexed: return "indexed";
    case PixelFormat::Format::RGBA: return "rgba";
    default: throw py::value_error{"unsupported format"};
    }
}

static auto TileFormat_encode(TileFormat& format, py::buffer buffer,
                              const TileFormat::offset_t offset, const Tile& tile) -> size_t
{
    auto view = requestWritable(buffer);
    return format.write(view, offset, tile);
}

static auto TileFormat_decode(TileFormat& format, py::buffer buffer,
                              const TileFormat::offset_t offset)
    -> std::pair<Tile, TileFormat::offset_t>
{
    auto const view = requestReadOnly(buffer);
    return format.read(view, offset);
}

static auto Tile_crop(const Tile& tile, const size_t x, const size_t y, const size_t width,
                      const size_t height) -> Tile
{
    TileRegion region{x, y, width, height};
    return tile.crop(region);
}

//static auto Tile_blit(Tile& tile, const size_t x, const size_t y, const Tile& source)

void registerKaizoGraphics(py::module_& m)
{
    py::class_<PixelFormat>(m, "_PixelFormat")
        .def_static("make_channels", &PixelFormat_make_channels)
        .def_static("make_indexed", &PixelFormat::makeIndexed)
        .def_property_readonly("kind", &PixelFormat_kind)
        .def_property_readonly("bits_per_pixel", &PixelFormat::bitsPerPixel)
        .def_property_readonly("bits_per_channel", &PixelFormat::bitsPerChannel)
        .def_property_readonly("channels", &PixelFormat::channels);

    py::class_<Palette>(m, "_Palette")
        .def(py::init<const PixelFormat, const size_t>())
        .def("get_color", &Palette::color)
        .def("set_color", &Palette::setColor)
        .def_property_readonly("color_format", &Palette::colorFormat)
        .def_property_readonly("index_format", &Palette::indexFormat);

    py::class_<Tile>(m, "_Tile")
        .def_static("load", &Tile_load)
        .def("save", &Tile_save)
        .def("crop", &Tile_crop)
        //.def("blit", &Tile_blot)
        .def("set_pixel",
             static_cast<void (Tile::*)(const size_t, const size_t, const Tile::pixel_t)>(
                 &Tile::setPixel))
        .def("get_pixel",
             static_cast<Tile::pixel_t (Tile::*)(const size_t, const size_t) const>(&Tile::pixel))
        .def_property_readonly("width", &Tile::width)
        .def_property_readonly("height", &Tile::height)
        .def_property_readonly("bits_per_pixel", &Tile::bitsPerPixel)
        .def_property_readonly("format", &Tile::format);

    py::class_<TileFormat>(m, "_TileFormat")
        .def_static("make", &TileFormat::make)
        .def("encode", &TileFormat_encode)
        .def("decode", &TileFormat_decode)
        .def("encoded_size", [](const TileFormat& format, const size_t count) {
            return format.requiredSize(count);
        });
}
