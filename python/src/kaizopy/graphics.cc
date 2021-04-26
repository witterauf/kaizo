#include "pyutilities.h"
#include <filesystem>
#include <kaizo/graphics/ImageFileFormat.h>
#include <kaizo/graphics/TileFormat.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
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

static auto Tile_blit(Tile& dest, const Tile& source, const size_t x, const size_t y,
                      const Tile::pixel_t bgColor)
{
    TileRegion region{0, 0, source.width(), source.height()};
    dest.blit(x, y, source, region, bgColor);
}

static auto Tile_bounding_box(const Tile& tile, const Tile::pixel_t bgColor)
    -> std::tuple<size_t, size_t, size_t, size_t>
{
    auto const bbox = tile.boundingBox(bgColor);
    return std::make_tuple(bbox.left(), bbox.top(), bbox.right(), bbox.bottom());
}

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
        .def(py::init<const size_t, const size_t, const PixelFormat>())
        .def_static("load", &Tile_load)
        .def("save", &Tile_save)
        .def("crop", &Tile_crop)
        .def("blit", &Tile_blit)
        .def("fill", &Tile::fill)
        .def("bounding_box", &Tile_bounding_box)
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

/*
static auto PyKaizoTile_blit(PyKaizoTile* self, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (nargs != 4)
    {
        PyErr_SetString(PyExc_TypeError,
                        "wrong number of arguments; expected 4 (source, x, y, bgColor)");
        return NULL;
    }

    if (!PyObject_IsInstance(args[0], (PyObject*)&PyKaizoTileType))
    {
        PyErr_SetString(PyExc_TypeError, "first argument must be a Tile");
        return NULL;
    }
    auto const* source = reinterpret_cast<PyKaizoTile*>(args[0]);

    TileRegion region{0, 0, source->tile.width(), source->tile.height()};

    auto x = PyLong_AsLongLong(args[1]);
    if (x == static_cast<decltype(x)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (x < 0)
    {
        region.setLeft(static_cast<size_t>(-x));
        region.setRight(source->tile.width());
        x = 0;
    }

    auto y = PyLong_AsLongLong(args[2]);
    if (y == static_cast<decltype(y)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (y < 0)
    {
        region.setTop(static_cast<size_t>(-y));
        region.setBottom(source->tile.height());
        y = 0;
    }

    auto const background = PyLong_AsUnsignedLongLong(args[3]);
    if (background == static_cast<decltype(background)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->tile.blit(static_cast<size_t>(x), static_cast<size_t>(y), source->tile, region,
                    static_cast<Tile::pixel_t>(background));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyKaizoTile_blit_partial(PyKaizoTile* self, PyObject* const* args,
                                     const Py_ssize_t nargs) -> PyObject*
{
    if (!pykCheckArguments(nargs, 8, "source, sx, sy, width, height, dx, dy, bgColor"))
    {
        return NULL;
    }

    auto* source = pykGetObject<PyKaizoTile>(args[0], &PyKaizoTileType);
    auto sx = pykGetNumber<long long>(args[1]);
    auto sy = pykGetNumber<long long>(args[2]);
    auto width = pykGetNumber<long long>(args[3]);
    auto height = pykGetNumber<long long>(args[4]);
    auto dx = pykGetNumber<long long>(args[5]);
    auto dy = pykGetNumber<long long>(args[6]);
    auto const bgColor = pykGetNumber<Tile::pixel_t>(args[7]);
    if (!source || !sx || !sy || !width || !height || !dx || !dy || !bgColor)
    {
        return NULL;
    }

    sx = std::max(*sx, static_cast<long long>(0));
    sy = std::max(*sy, static_cast<long long>(0));
    dx = std::max(*dx, static_cast<long long>(0));
    dy = std::max(*dy, static_cast<long long>(0));
    width = std::max(*width, static_cast<long long>(0));
    height = std::max(*height, static_cast<long long>(0));

    TileRegion region{static_cast<size_t>(*sx), static_cast<size_t>(*sy),
                      static_cast<size_t>(*width), static_cast<size_t>(*height)};
    self->tile.blit(static_cast<size_t>(*dx), static_cast<size_t>(*dy), source->tile, region,
                    *bgColor);
    return pykNone();
}

static auto PyKaizoTile_bounding_box(PyKaizoTile* self, PyObject* pyBgColor) -> PyObject*
{
    auto maybeBgColor = pykGetNumber<Tile::pixel_t>(pyBgColor);
    if (!maybeBgColor)
    {
        return NULL;
    }

    auto const bb = self->tile.boundingBox(*maybeBgColor);
    return Py_BuildValue("(KKKK)", static_cast<unsigned long long>(bb.left()),
                         static_cast<unsigned long long>(bb.top()),
                         static_cast<unsigned long long>(bb.right()),
                         static_cast<unsigned long long>(bb.bottom()));
}

*/