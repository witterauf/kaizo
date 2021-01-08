#include <fstream>
#include <kaizo/data/DataPath.h>
#include <kaizo/data/data/ArrayData.h>
#include <kaizo/data/data/IntegerData.h>
#include <kaizo/data/data/RecordData.h>
#include <kaizo/data/data/ReferenceData.h>
#include <kaizo/data/data/StringData.h>
#include <kaizo/data/serialization/CsvSerialization.h>
#include <kaizo/utilities/CsvReader.h>

using namespace kaizo::data;

namespace kaizo::data {

void CsvSerialization::serialize(const Data& data, const std::filesystem::path& sourcePath)
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

void CsvSerialization::serializeData(const Data& data)
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

void CsvSerialization::serializeRecord(const Data& data)
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

void CsvSerialization::serializeArray(const Data& data)
{
    auto const& arrayData = static_cast<const ArrayData&>(data);
    for (auto i = 0U; i < arrayData.elementCount(); ++i)
    {
        auto const& element = arrayData.element(i);
        m_currentPath /= DataPathElement::makeIndex(i + 1);
        serializeData(element);
        m_currentPath.goUp();
    }
}

void CsvSerialization::serializeInteger(const Data& data)
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

void CsvSerialization::serializeString(const Data& data)
{
    auto const& stringData = static_cast<const StringData&>(data);
    outputRow(DataType::String, stringData.value());
}

void CsvSerialization::outputRow(DataType type, const std::string& value)
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
    -> std::unique_ptr<Data>
{
    CsvReader reader{filename};
    while (auto maybeRow = reader.nextRow())
    {
        if (maybeRow->size() != 3)
        {
            throw std::runtime_error{"CsvSerialization: expecting CSV file with 3 columns in each "
                                     "row (path, type, value)"};
        }

        decodePath((*maybeRow)[0]);
        decodeType((*maybeRow)[1]);
        decodeValue(std::move((*maybeRow)[2]));

        if (!m_root)
        {
            processRoot();
        }
        else
        {
            processExisting(m_root.get(), 0);
        }
    }
    return std::move(m_root);
}

void CsvSerialization::processRoot()
{
    auto const& element = m_currentPath.element(0);
    switch (element.kind())
    {
    case DataPathElement::Kind::Index:
        m_root = std::make_unique<ArrayData>();
        processArray(m_root.get(), 0);
        break;
    case DataPathElement::Kind::Name:
        m_root = std::make_unique<RecordData>();
        processRecord(m_root.get(), 0);
        break;
    default: throw std::runtime_error{"CsvSerialization: root must be either an index or a name"};
    }
}

void CsvSerialization::decodePath(const std::string& path)
{
    if (auto maybePath = DataPath::fromString(path))
    {
        m_currentPath = *maybePath;
    }
    else
    {
        throw std::runtime_error{"CsvSerialization: '" + path + "' is not a valid path"};
    }
}

void CsvSerialization::decodeType(const std::string& type)
{
    if (auto maybeType = toDataType(type))
    {
        m_currentType = *maybeType;
    }
    else
    {
        throw std::runtime_error{"CsvSerialization: '" + type + "' is not a valid data type"};
    }
}

void CsvSerialization::decodeValue(std::string&& value)
{
    m_currentValue = std::move(value);
}

void CsvSerialization::process(Data* data, size_t pathIndex)
{
    auto const& element = m_currentPath.element(pathIndex);
    switch (element.kind())
    {
    case DataPathElement::Kind::Index: processArray(data, pathIndex); return;
    case DataPathElement::Kind::Name: processRecord(data, pathIndex); return;
    default: return;
    }
}

void CsvSerialization::processExisting(Data* data, size_t pathIndex)
{
    auto const& element = m_currentPath.element(pathIndex);
    switch (element.kind())
    {
    case DataPathElement::Kind::Index:
        if (data->type() != DataType::Array)
        {
            throw std::runtime_error{"CsvSerialization: types inconsistent (expected " +
                                     toString(data->type()) + ", but got an index)"};
        }
        processArray(data, pathIndex);
        break;
    case DataPathElement::Kind::Name:
        if (data->type() != DataType::Record)
        {
            throw std::runtime_error{"CsvSerialization: types inconsistent (expected " +
                                     toString(data->type()) + ", but got a name)"};
        }
        processRecord(data, pathIndex);
        break;
    default: return;
    }
}

void CsvSerialization::processArray(Data* data, size_t pathIndex)
{
    auto* arrayData = static_cast<ArrayData*>(data);
    // DataPaths use 1-based indices in arrays
    auto const index = m_currentPath.element(pathIndex).index() - 1;

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
    case DataType::Reference: return makeReferenceLeaf();
    default:
        throw std::runtime_error{"CsvSerialization: " + toString(m_currentType) +
                                 " is not a supported type for leaves"};
    }
}

auto CsvSerialization::makeReferenceLeaf() -> std::unique_ptr<Data>
{
    if (auto maybePath = DataPath::fromString(m_currentValue))
    {
        return std::make_unique<ReferenceData>(*maybePath);
    }
    else
    {
        throw std::runtime_error{"CsvSerialization: '" + m_currentValue +
                                 "' is not a supported reference"};
    }
}

auto CsvSerialization::makeIntegerLeaf() -> std::unique_ptr<Data>
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

auto CsvSerialization::makeStringLeaf() -> std::unique_ptr<Data>
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

auto CsvSerialization::makeNext(size_t pathIndex) -> std::unique_ptr<Data>
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

} // namespace kaizo::data