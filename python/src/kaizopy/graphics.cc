#include "graphics.h"
#include <filesystem>
#include <iostream>
#include <kaizo/graphics/ImageFileFormat.h>
#include <kaizo/graphics/TileFormat.h>
#include <string>

using namespace kaizo;
using namespace fuse;

//##[ Tile ]#######################################################################################

PyTypeObject PyKaizoTileType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._Tile"};

auto PyKaizoTile_New(Tile&& tile) -> PyObject*
{
    auto* pyTile = PyObject_New(PyKaizoTile, &PyKaizoTileType);
    new (&pyTile->tile) Tile{std::move(tile)};
    return (PyObject*)pyTile;
}

static int PyKaizoTile_init(PyKaizoTile* self, PyObject* args, PyObject*)
{
    unsigned long long width;
    unsigned long long height;
    PyObject* pyFormat{nullptr};
    if (PyArg_ParseTuple(args, "KKO", &width, &height, &pyFormat) < 0)
    {
        return -1;
    }

    if (pyFormat == nullptr || !PyObject_IsInstance(pyFormat, (PyObject*)&PyKaizoPixelFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "format must be a _PixelFormat");
        return -1;
    }

    auto const format = PixelFormat::fromIntegerRepresentation(
        reinterpret_cast<PyKaizoPixelFormat*>(pyFormat)->representation);
    new (&self->tile) Tile{width, height, format};
    return 0;
}

static auto PyKaizoTile_loadfromfile(PyKaizoTile*, PyObject* pyFilename) -> PyObject*
{
    const char* filename = PyUnicode_AsUTF8(pyFilename);
    if (!filename)
    {
        return NULL;
    }
    std::filesystem::path path{filename};
    if (!path.has_extension())
    {
        PyErr_SetString(PyExc_ValueError,
                        ("unsupported image file format: '" + path.string() + "'").c_str());
        return NULL;
    }

    auto const format = ImageFileFormat::makeFormat(path.extension().string().substr(1));
    if (!format)
    {
        PyErr_SetString(
            PyExc_ValueError,
            ("unsupported image file format: '" + path.extension().string() + "'").c_str());
        return NULL;
    }

    auto maybeImageWithPalette = format->load(path);
    if (!maybeImageWithPalette)
    {
        PyErr_SetString(PyExc_ValueError,
                        ("could not load image file '" + path.string() + "'").c_str());
        return NULL;
    }

    PyObject* pyTile = PyKaizoTile_New(std::move(maybeImageWithPalette->first));
    return Py_BuildValue("NO", pyTile, Py_None);
}

static auto PyKaizoTile_savetofile(PyKaizoTile* self, PyObject* pyFilename) -> PyObject*
{
    const char* filename = PyUnicode_AsUTF8(pyFilename);
    if (!filename)
    {
        return NULL;
    }
    std::filesystem::path path{filename};
    if (!path.has_extension())
    {
        PyErr_SetString(PyExc_ValueError,
                        ("unsupported image file format: '" + path.string() + "'").c_str());
        return NULL;
    }

    auto const format = ImageFileFormat::makeFormat(path.extension().string().substr(1));
    if (!format)
    {
        PyErr_SetString(
            PyExc_ValueError,
            ("unsupported image file format: '" + path.extension().string() + "'").c_str());
        return NULL;
    }

    if (!format->save(path, self->tile, nullptr))
    {
        PyErr_SetString(PyExc_ValueError,
                        ("could not save to image file '" + path.string() + "'").c_str());
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyKaizoTile_setpixel(PyKaizoTile* self, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (nargs != 3)
    {
        PyErr_SetString(PyExc_TypeError, "wrong number of arguments; expected 3 (x, y, value)");
        return NULL;
    }

    auto const x = PyLong_AsUnsignedLongLong(args[0]);
    if (x == static_cast<decltype(x)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const y = PyLong_AsUnsignedLongLong(args[1]);
    if (y == static_cast<decltype(y)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const value = PyLong_AsUnsignedLongLong(args[2]);
    if (value == static_cast<decltype(value)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->tile.setPixel(x, y, static_cast<Tile::pixel_t>(value));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyKaizoTile_getpixel(PyKaizoTile* self, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_TypeError, "wrong number of arguments; expected 2 (x, y)");
        return NULL;
    }

    auto const x = PyLong_AsUnsignedLongLong(args[0]);
    if (x == static_cast<decltype(x)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const y = PyLong_AsUnsignedLongLong(args[1]);
    if (y == static_cast<decltype(y)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    auto const value = self->tile.pixel(x, y);
    return PyLong_FromUnsignedLongLong(value);
}

static auto PyKaizoTile_crop(PyKaizoTile* self, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (nargs != 4)
    {
        PyErr_SetString(PyExc_TypeError,
                        "wrong number of arguments; expected 4 (x, y, width, height)");
        return NULL;
    }

    auto const x = PyLong_AsUnsignedLongLong(args[0]);
    if (x == static_cast<decltype(x)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const y = PyLong_AsUnsignedLongLong(args[1]);
    if (y == static_cast<decltype(y)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const width = PyLong_AsUnsignedLongLong(args[2]);
    if (width == static_cast<decltype(width)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const height = PyLong_AsUnsignedLongLong(args[3]);
    if (height == static_cast<decltype(height)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    auto tile = self->tile.crop(TileRegion{x, y, width, height});
    return PyKaizoTile_New(std::move(tile));
}

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

static auto PyKaizoTile_getwidth(PyKaizoTile* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(self->tile.width());
}

static auto PyKaizoTile_getheight(PyKaizoTile* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(self->tile.height());
}

static auto PyKaizoTile_getbpp(PyKaizoTile* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(self->tile.bitsPerPixel());
}

static auto PyKaizoTile_getformat(PyKaizoTile* self, void*) -> PyObject*
{
    auto* pyFormat = PyObject_New(PyKaizoPixelFormat, &PyKaizoPixelFormatType);
    pyFormat->representation = self->tile.format().integerRepresentation();
    return (PyObject*)pyFormat;
}

static void PyKaizoTile_dealloc(PyKaizoTile* self)
{
    self->tile.~Tile();
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyKaizoTile_methods[] = {
    {"load", (PyCFunction)PyKaizoTile_loadfromfile, METH_O | METH_STATIC},
    {"save", (PyCFunction)PyKaizoTile_savetofile, METH_O},
    {"set_pixel", (PyCFunction)PyKaizoTile_setpixel, METH_FASTCALL},
    {"get_pixel", (PyCFunction)PyKaizoTile_getpixel, METH_FASTCALL},
    {"crop", (PyCFunction)PyKaizoTile_crop, METH_FASTCALL},
    {"blit", (PyCFunction)PyKaizoTile_blit, METH_FASTCALL},
    {"blit_partial", (PyCFunction)PyKaizoTile_blit_partial, METH_FASTCALL},
    {"bounding_box", (PyCFunction)PyKaizoTile_bounding_box, METH_O},
    {NULL}};

PyGetSetDef PyKaizoTile_getsets[] = {
    {"width", (getter)PyKaizoTile_getwidth, NULL, "get the width of the tile", NULL},
    {"height", (getter)PyKaizoTile_getheight, NULL, "get the height of the tile", NULL},
    {"format", (getter)PyKaizoTile_getformat, NULL, "get the _PixelFormat of the tile", NULL},
    {"bits_per_pixel", (getter)PyKaizoTile_getbpp, NULL, "get the BPP of the tile", NULL},
    {NULL}};

static auto createKaizoTileType() -> PyTypeObject*
{
    PyKaizoTileType.tp_new = PyType_GenericNew;
    PyKaizoTileType.tp_basicsize = sizeof(PyKaizoTile);
    PyKaizoTileType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoTileType.tp_doc = "Represents a Tile with a given width, height, and bits per pixel";
    PyKaizoTileType.tp_methods = PyKaizoTile_methods;
    PyKaizoTileType.tp_init = (initproc)PyKaizoTile_init;
    PyKaizoTileType.tp_dealloc = (destructor)PyKaizoTile_dealloc;
    PyKaizoTileType.tp_getset = PyKaizoTile_getsets;
    return &PyKaizoTileType;
}

//##[ Palette ]####################################################################################

PyTypeObject PyKaizoPaletteType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._Palette"};

static int PyKaizoPalette_init(PyKaizoPalette* self, PyObject* args, PyObject*)
{
    PyObject* pyFormat{nullptr};
    unsigned long long colorCount{0};
    if (PyArg_ParseTuple(args, "OK", &pyFormat, &colorCount) < 0)
    {
        return -1;
    }
    if (colorCount == 0)
    {
        PyErr_SetString(PyExc_ValueError, "must have at least one color");
        return -1;
    }
    if (pyFormat == NULL || !PyObject_IsInstance(pyFormat, (PyObject*)&PyKaizoPixelFormatType))
    {
        PyErr_SetString(PyExc_TypeError, "format must be of type _PixelFormat");
        return -1;
    }

    auto const representation = reinterpret_cast<PyKaizoPixelFormat*>(pyFormat)->representation;
    auto const format = PixelFormat::fromIntegerRepresentation(representation);
    new (&self->palette) Palette{format, static_cast<size_t>(colorCount)};
    return 0;
}

static auto PyKaizoPalette_set_color(PyKaizoPalette* self, PyObject* const* args,
                                     const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_TypeError,
                        ("got " + std::to_string(nargs) +
                         "arguments, but expected 2 arguments (index, packed_color)")
                            .c_str());
        return NULL;
    }

    auto const index = PyLong_AsUnsignedLongLong(args[0]);
    if (index == static_cast<decltype(index)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (index >= self->palette.count())
    {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }

    auto const color = PyLong_AsUnsignedLongLong(args[1]);
    if (color == static_cast<decltype(color)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->palette.setColor(index, static_cast<Palette::color_t>(color));
    Py_INCREF(Py_None);
    return Py_None;
}

static auto PyKaizoPalette_color(PyKaizoPalette* self, PyObject* pyIndex) -> PyObject*
{
    auto const index = PyLong_AsUnsignedLongLong(pyIndex);
    if (index == static_cast<decltype(index)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    if (index >= self->palette.count())
    {
        PyErr_SetString(PyExc_IndexError, "no such color");
        return NULL;
    }
    auto const color = self->palette.color(index);
    return Py_BuildValue("K", static_cast<unsigned long long>(color));
}

static void PyKaizoPalette_dealloc(PyKaizoPalette* self)
{
    self->palette.~Palette();
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyKaizoPalette_methods[] = {
    {"color", (PyCFunction)PyKaizoPalette_color, METH_O},
    {"set_color", (PyCFunction)PyKaizoPalette_set_color, METH_FASTCALL},
    {NULL}};

static auto createKaizoPaletteType() -> PyTypeObject*
{
    PyKaizoPaletteType.tp_new = PyType_GenericNew;
    PyKaizoPaletteType.tp_basicsize = sizeof(PyKaizoPalette);
    PyKaizoPaletteType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoPaletteType.tp_doc = "Represents an indexed color palette";
    PyKaizoPaletteType.tp_methods = PyKaizoPalette_methods;
    PyKaizoPaletteType.tp_init = (initproc)PyKaizoPalette_init;
    PyKaizoPaletteType.tp_dealloc = (destructor)PyKaizoPalette_dealloc;
    return &PyKaizoPaletteType;
}

//##[ PixelFormat ]################################################################################

PyTypeObject PyKaizoPixelFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._PixelFormat"};

static auto PyKaizoPixelFormat_make_indexed_format(PyObject*, PyObject* pyBpp) -> PyObject*
{
    auto const bpp = PyLong_AsUnsignedLongLong(pyBpp);
    if (bpp == static_cast<decltype(bpp)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    auto* pyFormat = PyObject_New(PyKaizoPixelFormat, &PyKaizoPixelFormatType);
    pyFormat->representation =
        PixelFormat::makeIndexed(static_cast<uint8_t>(bpp)).integerRepresentation();
    return (PyObject*)pyFormat;
}

static auto PyKaizoPixelFormat_make_channels_format(PyObject*, PyObject* const* args,
                                                    const Py_ssize_t nargs) -> PyObject*
{
    if (nargs != 2)
    {
        PyErr_SetString(PyExc_TypeError,
                        ("got " + std::to_string(nargs) +
                         "arguments, but expected 2 arguments (channels, bits_per_channel)")
                            .c_str());
        return NULL;
    }

    const char* channels = PyUnicode_AsUTF8(args[0]);
    if (channels == nullptr)
    {
        return NULL;
    }
    auto const bpc = PyLong_AsUnsignedLongLong(args[1]);
    if (bpc == static_cast<decltype(bpc)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    if (std::string{channels} == "R.G.B.A" && bpc == 8)
    {
        auto* pyFormat = PyObject_New(PyKaizoPixelFormat, &PyKaizoPixelFormatType);
        pyFormat->representation = PixelFormat::rgba().integerRepresentation();
        return (PyObject*)pyFormat;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "invalid channel format");
        return NULL;
    }
}

static auto PyKaizoPixelFormat_bits_per_pixel(PyKaizoPixelFormat* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(
        PixelFormat::fromIntegerRepresentation(self->representation).bitsPerPixel());
}

static auto PyKaizoPixelFormat_bits_per_channel(PyKaizoPixelFormat* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(
        PixelFormat::fromIntegerRepresentation(self->representation).bitsPerChannel());
}

static auto PyKaizoPixelFormat_channels(PyKaizoPixelFormat* self, void*) -> PyObject*
{
    return PyLong_FromUnsignedLongLong(
        PixelFormat::fromIntegerRepresentation(self->representation).channels());
}

static auto PyKaizoPixelFormat_kind(PyKaizoPixelFormat* self, void*) -> PyObject*
{
    auto const format = PixelFormat::fromIntegerRepresentation(self->representation);
    switch (format.format())
    {
    case PixelFormat::Format::Indexed: return Py_BuildValue("s", "indexed");
    case PixelFormat::Format::RGBA: return Py_BuildValue("s", "rgba");
    default: PyErr_SetString(PyExc_ValueError, "unsupported PixelFormat"); return NULL;
    }
}

static PyMethodDef PyKaizoPixelFormat_methods[] = {
    {"make_indexed", (PyCFunction)PyKaizoPixelFormat_make_indexed_format, METH_O | METH_STATIC},
    {"make_channels", (PyCFunction)PyKaizoPixelFormat_make_channels_format,
     METH_FASTCALL | METH_STATIC},
    {NULL}};

PyGetSetDef PyKaizoPixelFormat_getsets[] = {
    {"kind", (getter)PyKaizoPixelFormat_kind, NULL, "get the basic format kind", NULL},
    {"bits_per_pixel", (getter)PyKaizoPixelFormat_bits_per_pixel, NULL, "get the bits per pixel",
     NULL},
    {"bits_per_channel", (getter)PyKaizoPixelFormat_bits_per_channel, NULL,
     "get the bits per channel", NULL},
    {"channels", (getter)PyKaizoPixelFormat_channels, NULL, "get the number of channels", NULL},
    {NULL}};

static auto createKaizoPixelFormatType() -> PyTypeObject*
{
    PyKaizoPixelFormatType.tp_new = PyType_GenericNew;
    PyKaizoPixelFormatType.tp_basicsize = sizeof(PyKaizoPixelFormat);
    PyKaizoPixelFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoPixelFormatType.tp_doc = "Represents pixel formats";
    PyKaizoPixelFormatType.tp_methods = PyKaizoPixelFormat_methods;
    PyKaizoPixelFormatType.tp_getset = PyKaizoPixelFormat_getsets;
    return &PyKaizoPixelFormatType;
}

//##[ TileFormat ]#################################################################################

PyTypeObject PyKaizoTileFormatType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy._TileFormat"};

static auto PyKaizoTileFormat_make(PyObject*, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (!pykCheckArguments(nargs, 2, "name, properties"))
    {
        return NULL;
    }

    auto const maybeName = pykGetString(args[0]);
    if (!maybeName)
    {
        return NULL;
    }

    if (!PyMapping_Check(args[1]))
    {
        PyErr_SetString(PyExc_TypeError, "properties must be a mapping from string to integers");
        return NULL;
    }
    PyObject* pyProperties = PyMapping_Items(args[1]);
    std::map<std::string, unsigned> properties;
    for (Py_ssize_t i = 0; i < PyList_Size(pyProperties); ++i)
    {
        PyObject* item = PyList_GetItem(pyProperties, i);
        PyObject* pyKey = PyTuple_GetItem(item, 0);
        PyObject* pyValue = PyTuple_GetItem(item, 1);
        auto const key = pykGetString(pyKey);
        auto const value = pykGetNumber<unsigned>(pyValue);
        if (!key || !value)
        {
            Py_DECREF(pyProperties);
            return NULL;
        }
        properties.insert(std::make_pair(*key, *value));
    }
    Py_DECREF(pyProperties);

    auto format = TileFormat::make(*maybeName, properties);
    if (!format)
    {
        PyErr_SetString(PyExc_ValueError, "unknown or invalid tile format");
        return NULL;
    }

    auto* pyFormat = PyObject_New(PyKaizoTileFormat, &PyKaizoTileFormatType);
    pyFormat->format = format.release();
    return (PyObject*)pyFormat;
}

static auto PyKaizoTileFormat_encode(PyKaizoTileFormat* self, PyObject* const* args,
                                     const Py_ssize_t nargs) -> PyObject*
{
    if (!pykCheckArguments(nargs, 3, "tile, binary, offset"))
    {
        return NULL;
    }

    auto* pyTile = pykGetObject<PyKaizoTile>(args[0], &PyKaizoTileType);
    if (!pyTile)
    {
        return NULL;
    }
    auto pyBuffer = pykGetWritableBuffer(args[1]);
    if (!pyBuffer)
    {
        return NULL;
    }
    auto const maybeOffset = pykGetNumber<size_t>(args[2]);
    if (!maybeOffset)
    {
        return NULL;
    }

    auto const newOffset = self->format->write(pyBuffer.mutableView(), *maybeOffset, pyTile->tile);
    return Py_BuildValue("K", static_cast<unsigned long long>(newOffset));
}

static auto PyKaizoTileFormat_decode(PyKaizoTileFormat* self, PyObject* const* args,
                                     const Py_ssize_t nargs) -> PyObject*
{
    if (!pykCheckArguments(nargs, 2, "binary, offset"))
    {
        return NULL;
    }

    auto pyBuffer = pykGetBuffer(args[0]);
    if (!pyBuffer)
    {
        return NULL;
    }

    auto const maybeOffset = pykGetNumber<size_t>(args[1]);
    if (!maybeOffset)
    {
        return NULL;
    }

    auto [tile, newOffset] = self->format->read(pyBuffer.view(), *maybeOffset);
    PyObject* pyTile = PyKaizoTile_New(std::move(tile));
    return Py_BuildValue("(OK)", pyTile, static_cast<unsigned long long>(newOffset));
}

static auto PyKaizoTileFormat_encoded_size(PyKaizoTileFormat* self, PyObject* pyCount) -> PyObject*
{
    auto const maybeCount = pykGetNumber<size_t>(pyCount);
    if (!maybeCount)
    {
        return NULL;
    }
    return Py_BuildValue("K",
                         static_cast<unsigned long long>(self->format->requiredSize(*maybeCount)));
}

static void PyKaizoTileFormat_dealloc(PyKaizoTileFormat* self)
{
    delete self->format;
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyKaizoTileFormat_methods[] = {
    {"make", (PyCFunction)PyKaizoTileFormat_make, METH_FASTCALL | METH_STATIC},
    {"encode", (PyCFunction)PyKaizoTileFormat_encode, METH_FASTCALL},
    {"decode", (PyCFunction)PyKaizoTileFormat_encode, METH_FASTCALL},
    {"encoded_size", (PyCFunction)PyKaizoTileFormat_encoded_size, METH_O},
    {NULL}};

static auto createKaizoTileFormatType() -> PyTypeObject*
{
    PyKaizoTileFormatType.tp_new = PyType_GenericNew;
    PyKaizoTileFormatType.tp_basicsize = sizeof(PyKaizoPixelFormat);
    PyKaizoTileFormatType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoTileFormatType.tp_doc = "Represents pixel formats";
    PyKaizoTileFormatType.tp_methods = PyKaizoTileFormat_methods;
    PyKaizoTileFormatType.tp_dealloc = (destructor)PyKaizoTileFormat_dealloc;
    return &PyKaizoTileFormatType;
}

//#################################################################################################

auto GraphicsModule::createTypes() -> std::map<std::string, PyTypeObject*>
{
    return {{"_TileFormat", createKaizoTileFormatType()},
            {"_Tile", createKaizoTileType()},
            {"_PixelFormat", createKaizoPixelFormatType()},
            {"_Palette", createKaizoPaletteType()}};
}
