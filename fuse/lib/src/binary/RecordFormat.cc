#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/RecordData.h>
#include <fuse/binary/RecordFormat.h>

namespace fuse::binary {

void RecordFormat::append(const std::string& name, std::unique_ptr<DataFormat>&& data)
{
    m_elements.push_back(Element{name, std::move(data)});
}

auto RecordFormat::elementCount() const -> size_t
{
    return m_elements.size();
}

auto RecordFormat::has(const std::string& name)
{
    return m_elements.cend() !=
           std::find_if(m_elements.cbegin(), m_elements.cend(),
                        [&name](auto const& element) { return element.name == name; });
}

auto RecordFormat::element(size_t index) const -> const Element&
{
    return m_elements[index];
}

auto RecordFormat::element(const std::string& name) -> const Element&
{
    Expects(has(name));
    auto iter = std::find_if(m_elements.cbegin(), m_elements.cend(),
                             [&name](auto const& element) { return element.name == name; });
    return *iter;
}

auto RecordFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    auto record = std::make_unique<RecordData>();
    for (auto const& element : m_elements)
    {
        reader.enter(DataPathElement::makeName(element.name));
        if (auto data = element.format->decode(reader))
        {
            record->set(element.name, std::move(data));
            reader.leave(record.get());
        }
        else
        {
            reader.leave(record.get());
            return {};
        }
    }
    return std::move(record);
}

void RecordFormat::doEncode(DataWriter& writer, const Data& data)
{
    if (data.type() != DataType::Record)
    {
        throw std::runtime_error{"type mismatch"};
    }
    auto const& recordData = static_cast<const RecordData&>(data);

    for (auto const& element : m_elements)
    {
        if (!recordData.has(element.name))
        {
            throw std::runtime_error{"type mismatch (no such record element)"};
        }
        writer.enter(DataPathElement::makeName(element.name));
        element.format->encode(writer, recordData.element(element.name));
        writer.leave();
    }
}

auto RecordFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto format = std::make_unique<RecordFormat>();
    for (auto const& element : m_elements)
    {
        format->m_elements.push_back({element.name, element.format->copy()});
    }
    copyDataFormat(*format);
    return std::move(format);
}

} // namespace fuse::binary