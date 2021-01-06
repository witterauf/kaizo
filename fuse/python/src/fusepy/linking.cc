#include "linking.h"
#include <fuse/addresses/Address.h>
#include <fuse/linking/Backtracker.h>

using namespace fuse;
namespace py = pybind11;

static auto BacktrackingPacker_get_link_offset(const BacktrackingPacker& packer, const size_t index)
    -> size_t
{
    if (index >= packer.objectCount())
    {
        throw py::index_error{"object index " + std::to_string(index) + " is out of range"};
    }
    auto const& object = *packer.object(index);
    if (!object.hasAllocation())
    {
        throw py::value_error{"object has no allocated offset"};
    }
    return object.allocation().offset;
}

static auto BacktrackingPacker_get_link_address(const BacktrackingPacker& packer,
                                                const size_t index) -> Address
{
    if (index >= packer.objectCount())
    {
        throw py::index_error{"object index " + std::to_string(index) + " is out of range"};
    }
    auto const& object = *packer.object(index);
    if (!object.hasAllocation())
    {
        throw py::value_error{"object has no allocated address"};
    }
    return object.allocation().address;
}

void registerLinkingTypes(pybind11::module_& m)
{
    py::class_<BacktrackingPacker>(m, "_BacktrackingPacker")
        .def("add_object",
             [](BacktrackingPacker& packer, const size_t size) {
                 auto const id = "obj_" + std::to_string(packer.objectCount());
                 packer.addObject(std::make_unique<LinkObject>(id, static_cast<size_t>(size)));
             })
        .def("add_free_block",
             [](BacktrackingPacker& packer, const size_t offset, const Address& address,
                const size_t size) {
                 packer.addFreeBlock(FreeBlock{offset, address, size});
             })
        .def("pack", &BacktrackingPacker::pack)
        .def("get_link_offset", &BacktrackingPacker_get_link_offset)
        .def("get_link_address", &BacktrackingPacker_get_link_address)
        .def("set_log_file", [](BacktrackingPacker& packer, const std::string& filename) {
            packer.setLogFile(filename);
        });
}
