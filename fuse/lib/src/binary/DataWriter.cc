#include <fuse/binary/DataWriter.h>

namespace fuse::binary {

auto DataWriter::binary() -> Binary&
{
    return m_partialSections.back();
}

void DataWriter::enter(const DataPathElement& element)
{
    m_path /= element;
}

void DataWriter::startSection()
{
    m_partialSections.push_back({});
}

void DataWriter::skip(size_t)
{
}

void DataWriter::endSection()
{
    m_partialSections.pop_back();
}

void DataWriter::leave()
{
    m_path.goUp();
}

} // namespace fuse::binary