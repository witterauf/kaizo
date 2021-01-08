#include "binary.h"
#include "fusepy.h"
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>

using namespace kaizo::data;
using namespace kaizo::data;
namespace py = pybind11;

static auto convert(const UnresolvedReference& ref) -> py::dict
{
    py::dict dict;
    dict["offset"] = ref.relativeOffset();
    dict["path"] = ref.referencedPath().toString();
    dict["layout"] = py::cast(&ref.addressLayout(), py::return_value_policy::reference);
    return dict;
}

static auto convert(const Object::Section& section) -> py::dict
{
    py::dict dict;
    dict["offset"] = section.offset;
    dict["actual_offset"] = section.realOffset;
    dict["size"] = section.size;
    return dict;
}

static auto convert(const Object& object) -> py::dict
{
    py::dict dict;
    dict["path"] = object.path().toString();
    if (object.hasFixedOffset())
    {
        dict["fixed_offset"] = object.fixedOffset();
    }
    if (object.alignment() != 1)
    {
        dict["alignment"] = object.alignment();
    }
    dict["actual_size"] = object.realSize();

    py::list unresolved(object.unresolvedReferenceCount());
    for (size_t i = 0; i < object.unresolvedReferenceCount(); ++i)
    {
        unresolved[i] = convert(object.unresolvedReference(i));
    }
    dict["unresolved"] = unresolved;

    py::list sections(object.sectionCount());
    for (size_t i = 0; i < object.sectionCount(); ++i)
    {
        sections[i] = convert(object.section(i));
    }
    dict["sections"] = sections;

    return dict;
}

static auto convert(const AnnotatedBinary& binary) -> py::list
{
    py::list list(binary.objectCount());
    for (size_t i = 0; i < binary.objectCount(); ++i)
    {
        auto obj = convert(*binary.object(i));
        auto const offset = static_cast<const PackedObject&>(*binary.object(i)).offset();
        PyObject* pyBinary = PyByteArray_FromStringAndSize(
            reinterpret_cast<const char*>(binary.binary().data() + offset),
            binary.object(i)->size());
        obj["binary"] = py::handle(pyBinary);
        list[i] = obj;
    }
    return list;
}

static auto DataWriter_assemble(DataWriter& writer) -> py::list
{
    auto const binary = writer.assemble();
    return convert(binary);
}

auto DataReader_init(py::buffer b) -> DataReader*
{
    auto const view = requestReadOnly(b);
    return new DataReader{view};
}

void registerFuseBinary(py::module_& m)
{
    py::class_<DataReader>(m, "_DataReader")
        .def(py::init(&DataReader_init))
        .def("set_offset", &DataReader::setOffset)
        .def("set_address_map",
             [](DataReader& reader, const AddressMap& map) { reader.setAddressMap(map.copy()); });

    py::class_<DataWriter>(m, "_DataWriter").def(py::init()).def("assemble", &DataWriter_assemble);
}