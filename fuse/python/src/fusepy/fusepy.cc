#include "fusepy.h"
#include "addresses.h"
#include "binary.h"
#include "dataformat.h"
#include "text.h"
#include <fuse/Binary.h>
#include <fuse/BinaryPatch.h>
#include <fuse/Integers.h>

namespace py = pybind11;
using namespace fuse;

auto requestReadOnly(py::buffer& b) -> BinaryView
{
    py::buffer_info info = b.request();
    if (info.ndim != 1)
    {
        throw std::runtime_error{"requires a 1-dimensional buffer"};
    }
    return BinaryView{reinterpret_cast<const uint8_t*>(info.ptr), static_cast<size_t>(info.size)};
}

auto requestWritable(py::buffer& b) -> MutableBinaryView
{
    py::buffer_info info = b.request();
    if (info.ndim != 1)
    {
        throw std::runtime_error{"requires a 1-dimensional buffer"};
    }
    if (info.readonly)
    {
        throw std::runtime_error{"requires a writable buffer"};
    }
    return MutableBinaryView{reinterpret_cast<uint8_t*>(info.ptr), static_cast<size_t>(info.size)};
}

static auto PyBinary_init(py::buffer b) -> Binary
{
    auto const view = requestReadOnly(b);
    return Binary::fromArray(view.data(), view.size());
}

static void BinaryPatch_apply(BinaryPatch& patch, py::buffer b, const size_t offset)
{
    auto view = requestWritable(b);
    patch.apply(view, offset);
}

PYBIND11_MODULE(fusepy, m)
{
    m.doc() = "ROM Hacking module";

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
        .def("effective_address",
             [](const BinaryPatch& patch, const size_t offset) {
                 return offset + patch.relativeOffset();
             })
        .def_property_readonly("is_partial",
                               [](const BinaryPatch& patch) { return !patch.usesOnlyFullBytes(); });

    py::enum_<Signedness>(m, "Signedness")
        .value("UNSIGNED", Signedness::Unsigned)
        .value("SIGNED", Signedness::Signed)
        .export_values();

    py::enum_<Endianness>(m, "Endianness")
        .value("LITTLE", Endianness::Little)
        .value("BIG", Endianness::Big)
        .export_values();

    registerFuseAddresses(m);
    registerFuseBinary(m);
    registerFuseText(m);
    registerDataFormatTypes(m);
}
