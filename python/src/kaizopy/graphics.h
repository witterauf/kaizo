#pragma once

#include <kaizo/graphics/PixelFormat.h>
#include <kaizo/graphics/Tile.h>

#ifdef _WIN32
#ifdef KAIZO_WIN_EXPORT
#define KAIZO_EXPORTED __declspec(dllexport)
#else
#define KAIZO_EXPORTED __declspec(dllimport)
#endif
#else
#define KAIZO_EXPORTED
#endif

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

struct PyKaizoTile
{
    PyObject_HEAD;
    kaizo::Tile tile;
};

extern KAIZO_EXPORTED PyTypeObject PyKaizoTileType;

bool registerKaizoGraphics(PyObject* module);
}