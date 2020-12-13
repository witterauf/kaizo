#include "graphics.h"
#include <filesystem>
#include <iostream>
#include <kaizo/graphics/ImageFileFormat.h>
#include <string>

using namespace kaizo;

//##[ Tile ]#######################################################################################

PyTypeObject PyKaizoTileType = {PyVarObject_HEAD_INIT(NULL, 0) "_kaizopy.Tile"};

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
    const char* format{nullptr};
    if (PyArg_ParseTuple(args, "KKs", &width, &height, &format) < 0)
    {
        return -1;
    }

    auto const maybeFormat = PixelFormat::fromString(format);
    if (!maybeFormat)
    {
        PyErr_SetString(PyExc_ValueError, "unsupported pixel format");
    }
    std::cout << maybeFormat->bitsPerChannel() << "\n";
    std::cout << maybeFormat->channels() << "\n";

    new (&self->tile) Tile{width, height, *maybeFormat};
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

    auto const x = PyLong_AsUnsignedLongLong(args[1]);
    if (x == static_cast<decltype(x)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const y = PyLong_AsUnsignedLongLong(args[2]);
    if (y == static_cast<decltype(y)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }
    auto const background = PyLong_AsUnsignedLongLong(args[3]);
    if (background == static_cast<decltype(background)>(-1) && PyErr_Occurred())
    {
        return NULL;
    }

    self->tile.blit(source->tile, x, y, background);
    Py_INCREF(Py_None);
    return Py_None;
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

static void PyKaizoTile_dealloc(PyKaizoTile* self)
{
    self->tile.~Tile();
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef PyKaizoTile_methods[] = {
    {"load_from_file", (PyCFunction)PyKaizoTile_loadfromfile, METH_O | METH_STATIC},
    {"save_to_file", (PyCFunction)PyKaizoTile_savetofile, METH_O},
    {"set_pixel", (PyCFunction)PyKaizoTile_setpixel, METH_FASTCALL},
    {"get_pixel", (PyCFunction)PyKaizoTile_getpixel, METH_FASTCALL},
    {"crop", (PyCFunction)PyKaizoTile_crop, METH_FASTCALL},
    {"blit", (PyCFunction)PyKaizoTile_blit, METH_FASTCALL},
    {NULL}};

PyGetSetDef PyKaizoTile_getsets[] = {
    {"width", (getter)PyKaizoTile_getwidth, NULL, "get the width of the tile", NULL},
    {"height", (getter)PyKaizoTile_getheight, NULL, "get the height of the tile", NULL},
    {"bits_per_pixel", (getter)PyKaizoTile_getbpp, NULL, "get the BPP of the tile", NULL},
    {NULL}};

static bool registerKaizoTile(PyObject* module)
{
    PyKaizoTileType.tp_new = PyType_GenericNew;
    PyKaizoTileType.tp_basicsize = sizeof(PyKaizoTile);
    PyKaizoTileType.tp_flags = Py_TPFLAGS_DEFAULT;
    PyKaizoTileType.tp_doc = "Represents a Tile with a given width, height, and bits per pixel";
    PyKaizoTileType.tp_methods = PyKaizoTile_methods;
    PyKaizoTileType.tp_init = (initproc)PyKaizoTile_init;
    PyKaizoTileType.tp_dealloc = (destructor)PyKaizoTile_dealloc;
    PyKaizoTileType.tp_getset = PyKaizoTile_getsets;

    if (PyType_Ready(&PyKaizoTileType) < 0)
    {
        return false;
    }
    Py_INCREF(&PyKaizoTileType);
    if (PyModule_AddObject(module, "Tile", (PyObject*)&PyKaizoTileType) < 0)
    {
        Py_DECREF(&PyKaizoTileType);
        return false;
    }
    return true;
}

//#################################################################################################

bool registerKaizoGraphics(PyObject* module)
{
    if (!registerKaizoTile(module))
    {
        return false;
    }
    return true;
}
