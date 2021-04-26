#include "pyutilities.h"
#include <kaizo/graphics/TileFormat.h>
#include <kaizo/systems/psp/TextureSwizzle.h>
#include <kaizo/systems/psp/TileFormats.h>
#include <pybind11/pybind11.h>
extern "C" {
#include <eboot.h>
}

namespace py = pybind11;
using namespace kaizo;

static auto psp_swizzle(py::buffer b, const size_t width, const size_t start, const size_t end)
    -> Binary
{
    auto const view = requestReadOnly(b);
    return swizzle(view, width, start, end);
}

static auto psp_unswizzle(py::buffer b, const size_t width, const size_t start, const size_t end)
    -> Binary
{
    auto const view = requestReadOnly(b);
    return unswizzle(view, width, start, end);
}

static auto psp_encrypt_eboot_bin(py::buffer b, const size_t tag) -> Binary
{
    auto const view = requestReadOnly(b);
    Binary encrypted(view.size());
    sign_eboot(const_cast<uint8_t*>(view.data()), view.size(), tag, encrypted.data());
    return encrypted;
}

void registerKaizoSystems(py::module_& m)
{
    m.def("_psp_swizzle", &psp_swizzle)
        .def("_psp_unswizzle", &psp_unswizzle)
        .def("_psp_encrypt_eboot_bin", &psp_encrypt_eboot_bin);
    TileFormat::registerTileFormat("psp.4bpp", std::make_unique<Psp4bppTileFormatFactory>());
}
