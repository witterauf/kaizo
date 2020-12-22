#pragma once

#include <kaizo/graphics/Palette.h>
#include <kaizo/graphics/PixelFormat.h>
#include <kaizo/graphics/Tile.h>
#include <kaizo/graphics/TileFormat.h>
#include <optional>
#include <utility>
#include <vector>

#include "kaizomodule.h"
#include "pyutilities.h"

class GraphicsModule : public KaizoModule
{
public:
    auto createTypes() -> std::map<std::string, PyTypeObject*> override;
};

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyKaizoTile
{
    PyObject_HEAD;
    kaizo::Tile tile;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoTileType;

struct PyKaizoPalette
{
    PyObject_HEAD;
    kaizo::Palette palette;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoPaletteType;

struct PyKaizoPixelFormat
{
    PyObject_HEAD;
    kaizo::PixelFormat::repr_t representation;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoPixelFormatType;

struct PyKaizoTileFormat
{
    PyObject_HEAD;
    kaizo::TileFormat* format;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoTileFormatType;
}