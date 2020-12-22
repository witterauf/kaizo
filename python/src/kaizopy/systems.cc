#include "systems.h"
#include <kaizo/graphics/TileFormat.h>
#include <kaizo/systems/psp/TextureSwizzle.h>
#include <kaizo/systems/psp/TileFormats.h>

using namespace kaizo;

static auto psp_swizzle(PyObject*, PyObject* const* args, const Py_ssize_t nargs) -> PyObject*
{
    if (!pykCheckArguments(nargs, 4, "binary, width, start, end"))
    {
        return NULL;
    }

    auto const maybeEnd = pykGetNumber<size_t>(args[3]);
    auto const maybeStart = pykGetNumber<size_t>(args[2]);
    auto const maybeWidth = pykGetNumber<unsigned int>(args[1]);
    if (!maybeWidth || !maybeStart || !maybeEnd)
    {
        return NULL;
    }

    auto pyBuffer = pykGetBuffer(args[0]);
    if (!pyBuffer)
    {
        return NULL;
    }

    auto const view = pyBuffer.view();
    auto const binary = swizzle(view, *maybeWidth, *maybeStart, *maybeEnd);
    return Py_BuildValue("y#", binary.data(), binary.size());
}

static auto psp_unswizzle(PyObject*, PyObject* const* args, const Py_ssize_t nargs)
    -> PyObject*
{
    if (!pykCheckArguments(nargs, 4, "binary, width, start, end"))
    {
        return NULL;
    }

    auto const maybeEnd = pykGetNumber<size_t>(args[3]);
    auto const maybeStart = pykGetNumber<size_t>(args[2]);
    auto const maybeWidth = pykGetNumber<unsigned int>(args[1]);
    if (!maybeWidth || !maybeStart || !maybeEnd)
    {
        return NULL;
    }

    auto pyBuffer = pykGetBuffer(args[0]);
    if (!pyBuffer)
    {
        return NULL;
    }

    auto const view = pyBuffer.view();
    auto const binary = unswizzle(view, *maybeWidth, *maybeStart, *maybeEnd);
    return Py_BuildValue("y#", binary.data(), binary.size());
}

//#################################################################################################

bool SystemsModule::initialize()
{
    TileFormat::registerTileFormat("psp.4bpp", std::make_unique<Psp4bppTileFormatFactory>());
    return true;
}

auto SystemsModule::createFunctions() -> std::vector<PyMethodDef>
{
    return {
        PyMethodDef{"_psp_swizzle", (PyCFunction)psp_swizzle, METH_FASTCALL,
                    "swizzles a texture as expected by PSP GPU"},
        PyMethodDef{"_psp_unswizzle", (PyCFunction)psp_unswizzle, METH_FASTCALL,
                    "unswizzles a PSP texture"},
    };
}
