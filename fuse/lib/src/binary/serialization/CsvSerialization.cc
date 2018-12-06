#include <fstream>
#include <fuse/binary/DataPath.h>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/RecordData.h>
#include <fuse/binary/data/StringData.h>
#include <fuse/binary/serialization/CsvSerialization.h>
#include <fuse/utilities/CsvReader.h>

using namespace fuse::binary;

namespace fuse {

void CsvSerialization::serialize(const binary::Data& data, const std::filesystem::path& sourcePath)
{
    m_currentPath.clear();
    std::ofstream output{sourcePath};
    if (output.good())
    {
        m_output = &output;
        serializeData(data);
    }
    else
    {
        throw std::runtime_error{"CsvSerialization: could not open file for writing"};
    }
}

void CsvSerialization::serializeData(const binary::Data& data)
{
    switch (data.type())
    {
    case DataType::Array: serializeArray(data); return;
    case DataType::Record: serializeRecord(data); return;
    case DataType::Integer: serializeInteger(data); return;
    case DataType::String: serializeString(data); return;
    default: throw std::runtime_error{"CsvSerialization: unsupported data type"};
    }
}

void CsvSerialization::serializeRecord(const binary::Data& data)
{
    auto const& recordData = static_cast<const RecordData&>(data);
    auto const elements = recordData.elementNames();
    for (auto const& name : elements)
    {
        auto const& element = recordData.element(name);
        m_currentPath /= DataPathElement::makeName(name);
        serializeData(element);
        m_currentPath.goUp();
    }
}

void CsvSerialization::serializeArray(const binary::Data& data)
{
    auto const& arrayData = static_cast<const ArrayData&>(data);
    for (auto i = 0U; i < arrayData.elementCount(); ++i)
    {
        auto const& element = arrayData.element(i);
        m_currentPath /= DataPathElement::makeIndex(i);
        serializeData(element);
        m_currentPath.goUp();
    }
}

void CsvSerialization::serializeInteger(const binary::Data& data)
{
    auto const& integerData = static_cast<const IntegerData&>(data);
    if (integerData.isNegative())
    {
        outputRow(DataType::Integer, std::to_string(integerData.asSigned()));
    }
    else
    {
        outputRow(DataType::Integer, std::to_string(integerData.asUnsigned()));
    }
}

void CsvSerialization::serializeString(const binary::Data& data)
{
    auto const& stringData = static_cast<const StringData&>(data);
    outputRow(DataType::String, stringData.value());
}

void CsvSerialization::outputRow(binary::DataType type, const std::string& value)
{
    (*m_output) << m_currentPath.toString() << "," << toString(type);
    if (type == DataType::String)
    {
        (*m_output) << ",\"" << value << "\"\n";
    }
    else
    {
        (*m_output) << "," << value << "\n";
    }
}

auto CsvSerialization::deserialize(const std::filesystem::path& filename)
    -> std::unique_ptr<binary::Data>
{
    auto root = std::make_unique<RecordData>();
    CsvReader reader{filename};
    while (auto maybeRow = reader.nextRow())
    {
        if (maybeRow->size() != 3)
        {
            throw std::runtime_error{
                "CsvSerialization expects CSV file with 3 columns in each row"};
        }
        if (auto maybePath = DataPath::fromString((*maybeRow)[0]))
        {
            m_currentPath = *maybePath;
        }
        else
        {
            throw std::runtime_error{"CsvSerialization: invalid path"};
        }
        if (auto maybeType = toDataType((*maybeRow)[1]))
        {
            m_currentType = *maybeType;
        }
        else
        {
            throw std::runtime_error{"CsvSerialization: invalid data type"};
        }
        m_currentValue = std::move((*maybeRow)[2]);
        processExisting(root.get(), 0);
    }
    return std::move(root);
}

void CsvSerialization::process(binary::Data* data, size_t pathIndex)
{
    auto const& element = m_currentPath.element(pathIndex);
    switch (element.kind())
    {
    case DataPathElement::Kind::Index: processArray(data, pathIndex); return;
    case DataPathElement::Kind::Name: processRecord(data, pathIndex); return;
    default: return;
    }
}

void CsvSerialization::processExisting(binary::Data* data, size_t pathIndex)
{
    auto const& element = m_currentPath.element(pathIndex);
    switch (element.kind())
    {
    case DataPathElement::Kind::Index:
        if (data->type() != DataType::Array)
        {
            throw std::runtime_error{"CsvSerialization: types inconsistent"};
        }
        processArray(data, pathIndex);
        break;
    case DataPathElement::Kind::Name:
        if (data->type() != DataType::Record)
        {
            throw std::runtime_error{"CsvSerialization: types inconsistent"};
        }
        processRecord(data, pathIndex);
        break;
    default: return;
    }
}

void CsvSerialization::processArray(Data* data, size_t pathIndex)
{
    auto* arrayData = static_cast<ArrayData*>(data);
    auto const index = m_currentPath.element(pathIndex).index();

    if (arrayData->hasElement(index))
    {
        processExisting(&arrayData->element(index), pathIndex + 1);
    }
    else
    {
        auto element = makeNext(pathIndex);
        arrayData->set(index, std::move(element));
    }
}

auto CsvSerialization::makeLeaf() -> std::unique_ptr<Data>
{
    switch (m_currentType)
    {
    case DataType::Integer: return makeIntegerLeaf();
    case DataType::String: return makeStringLeaf();
    default: return {};
    }
}

auto CsvSerialization::makeIntegerLeaf() -> std::unique_ptr<binary::Data>
{
    if (m_currentValue[0] == '-')
    {
        auto const value = std::stoll(m_currentValue);
        return std::make_unique<IntegerData>(value);
    }
    else
    {
        auto const value = std::stoull(m_currentValue);
        return std::make_unique<IntegerData>(value);
    }
}

auto CsvSerialization::makeStringLeaf() -> std::unique_ptr<binary::Data>
{
    return std::make_unique<StringData>(m_currentValue);
}

void CsvSerialization::processRecord(Data* data, size_t pathIndex)
{
    auto* recordData = static_cast<RecordData*>(data);
    auto const name = m_currentPath.element(pathIndex).name();

    if (recordData->has(name))
    {
        processExisting(&recordData->element(name), pathIndex + 1);
    }
    else
    {
        auto element = makeNext(pathIndex);
        recordData->set(name, std::move(element));
    }
}

auto CsvSerialization::makeNext(size_t pathIndex) -> std::unique_ptr<binary::Data>
{
    if (pathIndex + 1 == m_currentPath.length())
    {
        return makeLeaf();
    }
    else if (m_currentPath.element(pathIndex + 1).isName())
    {
        auto element = std::make_unique<RecordData>();
        process(element.get(), pathIndex + 1);
        return std::move(element);
    }
    else if (m_currentPath.element(pathIndex + 1).isIndex())
    {
        auto element = std::make_unique<ArrayData>();
        process(element.get(), pathIndex + 1);
        return std::move(element);
    }
    else
    {
        throw std::runtime_error{"CsvSerialization: unsupported path"};
    }
}

} // namespace fuse