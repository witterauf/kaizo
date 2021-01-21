#include "kaizopy.h"
#include "pyutilities.h"
#include <kaizo/binary/Binary.h>
#include <kaizo/binary/BinaryPatch.h>
#include <optional>
#include <pybind11/pybind11.h>

using namespace kaizo;
namespace py = pybind11;

static auto PyBinary_init(py::buffer b) -> Binary
{
    auto const view = requestReadOnly(b);
    return Binary::fromArray(view.data(), view.size());
}

static void BinaryPatch_apply(BinaryPatch& patch, py::buffer b, const size_t offset)
{
    auto view = requestWritable(b);
    if (offset + patch.relativeOffset() + patch.size() > view.size())
    {
        throw py::index_error("patch exceeds buffer size");
    }
    patch.apply(view, offset);
}

PYBIND11_MODULE(kaizopy, m)
{
    m.doc() = "ROM hacking tools";

    py::class_<Binary>(m, "Binary", py::buffer_protocol())
        .def(py::init(&PyBinary_init))
        .def_buffer([](Binary& binary) -> py::buffer_info {
            return py::buffer_info(binary.data(),                            // data pointer
                                   sizeof(uint8_t),                          // element size
                                   py::format_descriptor<uint8_t>::format(), // Python format string
                                   1,                                        // dimensions
                                   {binary.size()},                          // dimension sizes
                                   {1}                                       // strides
            );
        });

    py::class_<BinaryPatch>(m, "BinaryPatch")
        .def("apply", &BinaryPatch_apply)
        .def("set_relative_offset", &BinaryPatch::setRelativeOffset)
        .def("effective_offset",
             [](const BinaryPatch& patch, const size_t offset) {
                 return offset + patch.relativeOffset();
             })
        .def_property_readonly("is_partial",
                               [](const BinaryPatch& patch) { return !patch.usesOnlyFullBytes(); })
        .def("__len__", &BinaryPatch::size);

    py::enum_<Signedness>(m, "Signedness")
        .value("UNSIGNED", Signedness::Unsigned)
        .value("SIGNED", Signedness::Signed)
        .export_values();

    py::enum_<Endianness>(m, "Endianness")
        .value("LITTLE", Endianness::Little)
        .value("BIG", Endianness::Big)
        .export_values();

    registerKaizoAddresses(m);
    registerKaizoData(m);
    registerKaizoGraphics(m);
    registerKaizoText(m);
    registerKaizoSystems(m);
}