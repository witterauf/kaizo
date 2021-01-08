#include "dataformat.h"
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/AddressLayout.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/NullData.h>
#include <fuse/binary/data/RecordData.h>
#include <fuse/binary/data/StringData.h>
#include <fuse/binary/formats/ArrayFormat.h>
#include <fuse/binary/formats/DataFormat.h>
#include <fuse/binary/formats/IntegerFormat.h>
#include <fuse/binary/formats/PointerFormat.h>
#include <fuse/binary/formats/RecordFormat.h>
#include <fuse/binary/formats/StringFormat.h>
#include <fuse/text/TextEncoding.h>

namespace py = pybind11;
using namespace kaizo::data;
using namespace kaizo::data;

static auto convert(const Data& data) -> py::object;

static auto convert(const IntegerData& data) -> py::int_
{
    py::int_ value;
    if (data.isNegative())
    {
        value = data.asSigned();
    }
    else
    {
        value = data.asUnsigned();
    }
    return value;
}

static auto convert(const StringData& data) -> py::str
{
    return py::str(data.value());
}

static auto convert(const ArrayData& data) -> py::list
{
    py::list list(data.elementCount());
    for (size_t i = 0; i < data.elementCount(); ++i)
    {
        list[i] = convert(data.element(i));
    }
    return list;
}

static auto convert(const RecordData& data) -> py::dict
{
    py::dict dict;
    auto const names = data.elementNames();
    for (auto const name : names)
    {
        dict[name.c_str()] = convert(data.element(name));
    }
    return dict;
}

static auto convert(const Data& data) -> py::object
{
    switch (data.type())
    {
    case DataType::Integer: return convert(static_cast<const IntegerData&>(data));
    case DataType::String: return convert(static_cast<const StringData&>(data));
    case DataType::Array: return convert(static_cast<const ArrayData&>(data));
    case DataType::Record: return convert(static_cast<const RecordData&>(data));
    default: throw std::runtime_error{"unsupported data type"};
    }
}

static auto convert(py::object object) -> std::unique_ptr<Data>;

static auto convertDict(py::dict dict) -> std::unique_ptr<RecordData>
{
    auto record = std::make_unique<RecordData>();
    for (auto item : dict)
    {
        auto const name = py::cast<std::string>(item.first);
        auto value = convert(py::reinterpret_borrow<py::object>(item.second));
        record->set(name, std::move(value));
    }
    return record;
}

static auto convertList(py::sequence list) -> std::unique_ptr<ArrayData>
{
    auto array = std::make_unique<ArrayData>();
    for (auto item : list)
    {
        array->append(convert(item));
    }
    return array;
}

static auto convertInt(py::object int_) -> std::unique_ptr<IntegerData>
{
    int overflow;
    auto const number = PyLong_AsLongLongAndOverflow(int_.ptr(), &overflow);
    if (overflow == 1)
    {
        auto const unsignedNumber = PyLong_AsUnsignedLongLong(int_.ptr());
        if (unsignedNumber == static_cast<decltype(unsignedNumber)>(-1) && PyErr_Occurred())
        {
            throw py::error_already_set{};
        }
        return std::make_unique<IntegerData>(unsignedNumber);
    }
    else if (overflow == -1)
    {
        throw py::value_error{"underflow: value too small for target type"};
    }
    else
    {
        if (number == -1 && PyErr_Occurred())
        {
            throw py::error_already_set{};
        }
        else if (number < 0)
        {
            return std::make_unique<IntegerData>(number);
        }
        else
        {
            return std::make_unique<IntegerData>(static_cast<unsigned long long>(number));
        }
    }
}

static auto convert(py::object object) -> std::unique_ptr<Data>
{
    if (py::isinstance<py::sequence>(object))
    {
        return convertList(py::reinterpret_borrow<py::sequence>(object));
    }
    else if (py::isinstance<py::dict>(object))
    {
        return convertDict(py::reinterpret_borrow<py::dict>(object));
    }
    else if (py::isinstance<py::str>(object))
    {
        return std::make_unique<StringData>(py::cast<std::string>(object));
    }
    else if (py::isinstance<py::int_>(object))
    {
        return convertInt(object);
    }
    else
    {
        throw py::type_error{"unsupported object type"};
    }
}

static auto DataFormat_encode(DataFormat& format, DataWriter& writer, py::object data,
                              const std::string& path)
{
    auto const maybePath = DataPath::fromString(path);
    if (!maybePath)
    {
        throw py::value_error("invalid data path");
    }
    writer.startData(*maybePath);
    try
    {
        format.encode(writer, *convert(data));
        writer.finishData();
    }
    catch (std::exception& e)
    {
        writer.abortData();
        throw e;
    }
}

void registerDataFormatTypes(py::module_& m)
{
    py::class_<DataFormat>(m, "_DataFormat")
        .def("set_skip_after", &DataFormat::setSkipAfter)
        .def("set_skip_before", &DataFormat::setSkipBefore)
        .def("set_fixed_offset", &DataFormat::setFixedOffset)
        .def("set_alignment", &DataFormat::setAlignment)
        .def("set_tag", &DataFormat::setTag)
        .def("decode",
             [](DataFormat& format, DataReader& reader) { return convert(*format.decode(reader)); })
        .def("encode", &DataFormat_encode);

    py::class_<IntegerFormat, DataFormat>(m, "_IntegerFormat")
        .def(py::init<size_t, Signedness, Endianness>());

    py::class_<StringFormat, DataFormat>(m, "_StringFormat")
        .def(py::init<const std::shared_ptr<text::TextEncoding>&>());

    py::class_<ArrayFormat, DataFormat>(m, "_ArrayFormat")
        .def(py::init())
        .def("set_element_format",
             [](ArrayFormat& format, const DataFormat& elementFormat) {
                 format.setElementFormat(elementFormat.copy());
             })
        .def("set_fixed_length", [](ArrayFormat& format, const size_t length) {
            format.setSizeProvider(std::make_unique<FixedSizeProvider>(length));
        });

    py::class_<RecordFormat, DataFormat>(m, "_RecordFormat")
        .def(py::init())
        .def("append",
             [](RecordFormat& format, const std::string& name, const DataFormat& elementFormat) {
                 format.append(name, elementFormat.copy());
             });

    py::class_<PointerFormat, DataFormat>(m, "_PointerFormat")
        .def(py::init())
        .def("set_pointee_format",
             [](PointerFormat& format, const DataFormat& pointeeFormat) {
                 format.setPointedFormat(pointeeFormat.copy());
             })
        .def("set_null_pointer", &PointerFormat::setNullPointer)
        .def("set_address_format", &PointerFormat::setAddressFormat)
        .def("use_address_map", &PointerFormat::useAddressMap)
        .def("set_address_layout", [](PointerFormat& format, const AddressLayout& layout) {
            format.setLayout(layout.copy());
        });
}
