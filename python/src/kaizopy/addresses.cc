#include "addresses.h"
#include "binary.h"
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/MipsLayout.h>
#include <fuse/addresses/RegionAddressMap.h>
#include <fuse/addresses/RelativeOffsetLayout.h>
#include <pybind11/stl.h>

using namespace kaizo::data;
namespace py = pybind11;

auto AddressLayout_encode(const AddressLayout& layout, const Address& address) -> py::list
{
    auto patches = layout.writeAddress(address);
    py::list list(patches.size());
    for (size_t i = 0; i < patches.size(); ++i)
    {
        list[i] = std::move(patches[i]);
    }
    return list;
}

void RelativeAddressLayout_set_layout(RelativeOffsetLayout& layout, const size_t size,
                                      const Signedness signedness, const Endianness endianness)
{
    IntegerLayout integerLayout;
    integerLayout.sizeInBytes = size;
    integerLayout.signedness = signedness;
    integerLayout.endianness = endianness;
    layout.setOffsetFormat(integerLayout);
}

auto RelativeAddressLayout_to_dict(const RelativeOffsetLayout& layout) -> py::dict
{
    py::dict dict;
    if (layout.hasNullPointer())
    {
        py::dict pyNullPointer;
        pyNullPointer["offset"] = layout.nullPointer().offset;
        pyNullPointer["address"] = layout.nullPointer().address;
        dict["null_pointer"] = pyNullPointer;
    }
    dict["base"] = layout.baseAddress();

    py::dict pyIntegerLayout;
    pyIntegerLayout["size"] = layout.offsetLayout().sizeInBytes;
    pyIntegerLayout["endianness"] = layout.offsetLayout().endianness;
    pyIntegerLayout["signedness"] = layout.offsetLayout().signedness;
    dict["layout"] = pyIntegerLayout;

    return dict;
}

static auto AddressMap_map_to_sources(const AddressMap& map, const Address address) -> py::list
{
    auto const sources = map.toSourceAddresses(address);
    py::list list(sources.size());
    for (size_t i = 0; i < sources.size(); ++i)
    {
        list[i] = sources[i];
    }
    return list;
}

//#################################################################################################

void registerFuseAddresses(py::module_& m)
{
    py::class_<Address>(m, "Address")
        .def("__str__", &Address::toString)
        .def("offset", &Address::applyOffset);
    py::class_<AddressFormat>(m, "_AddressFormat").def("from_int", &AddressFormat::fromInteger);
    m.add_object("_FileOffset", py::cast(static_cast<const AddressFormat*>(fileOffsetFormat()),
                                         py::return_value_policy::reference));

    py::class_<AddressMap>(m, "_AddressMap")
        .def("map_to_sources", &AddressMap_map_to_sources)
        .def("map_to_target", [](const AddressMap& map, const Address address) {
            return map.toTargetAddress(address);
        });
    py::class_<RegionAddressMap, AddressMap>(m, "_RegionedAddressMap")
        .def(py::init<const AddressFormat*, const AddressFormat*>())
        .def("add_region", &RegionAddressMap::map);

    py::class_<AddressLayout>(m, "_AddressLayout")
        .def("encode", &AddressLayout_encode)
        .def_property_readonly("id", &AddressLayout::getName);
    py::class_<RelativeOffsetLayout, AddressLayout>(m, "_RelativeAddressLayout")
        .def(py::init())
        .def("set_layout", &RelativeAddressLayout_set_layout)
        .def("set_null_pointer", &RelativeOffsetLayout::setNullPointer)
        .def("set_fixed_base_address", &RelativeOffsetLayout::setBaseAddress)
        .def("to_dict", &RelativeAddressLayout_to_dict);
    py::class_<MipsLayout, AddressLayout>(m, "_MipsEmbeddedLayout")
        .def(py::init())
        .def("set_base_address", &MipsLayout::setBaseAddress)
        .def("set_offsets", &MipsLayout::setOffsets);
}
