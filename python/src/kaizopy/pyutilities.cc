#include "pyutilities.h"

namespace py = pybind11;
using namespace kaizo;

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
