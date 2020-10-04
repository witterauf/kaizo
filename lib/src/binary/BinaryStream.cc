#include "kaizo/binary/BinaryStream.h"
#include <diagnostics/Contracts.h>

namespace kaizo {

using namespace fuse;

BinaryStream::BinaryStream(const std::filesystem::path& filename, Mode mode)
    : m_mode{mode}
{
    switch (mode)
    {
    case Mode::Input:
        m_stream = std::fstream{filename, std::fstream::binary | std::fstream::in};
        break;
    case Mode::Output:
        m_stream = std::fstream{filename, std::fstream::binary | std::fstream::out};
        break;
    case Mode::InputOutput:
        m_stream =
            std::fstream{filename, std::fstream::binary | std::fstream::out | std::fstream::in};
        break;
    default: InvalidCase(mode);
    }
}

void BinaryStream::setEndianness(Endianness endianness)
{
    m_endianness = endianness;
}

void BinaryStream::setLittleEndian()
{
    m_endianness = Endianness::Little;
}

void BinaryStream::setBigEndian()
{
    m_endianness = Endianness::Big;
}

void BinaryStream::seek(size_t offset)
{
    if (m_mode == Mode::Input)
    {
        m_stream.seekg(offset);
    }
    else if (m_mode == Mode::Output)
    {
        m_stream.seekp(offset);
    }
    else if (m_mode == Mode::InputOutput)
    {
        m_stream.seekp(offset);
        m_stream.seekg(offset);
    }
}

auto BinaryStream::writeOffset() -> size_t
{
    return m_stream.tellp();
}

auto BinaryStream::size() -> size_t
{
    if (m_mode == Mode::Output)
    {
        auto const offset = m_stream.tellp();
        m_stream.seekp(0, std::fstream::end);
        auto const size = m_stream.tellp();
        m_stream.seekp(offset);
        return size;
    }
    else
    {
        auto const offset = m_stream.tellg();
        m_stream.seekg(0, std::fstream::end);
        auto const size = m_stream.tellg();
        m_stream.seekg(offset);
        return size;
    }
}

void BinaryStream::write(uint8_t value)
{
    m_stream.write(reinterpret_cast<const char*>(&value), 1);
}

void BinaryStream::write(char value)
{
    m_stream.write(reinterpret_cast<const char*>(&value), 1);
}

void BinaryStream::write(uint16_t value)
{
    if (m_endianness == Endianness::Little)
    {
        m_stream.write(reinterpret_cast<const char*>(&value), 2);
    }
    else
    {
        write(static_cast<uint8_t>(value >> 8));
        write(static_cast<uint8_t>(value & 0xff));
    }
}

void BinaryStream::write(uint32_t value)
{
    if (m_endianness == Endianness::Little)
    {
        m_stream.write(reinterpret_cast<const char*>(&value), 4);
    }
    else
    {
        write(static_cast<uint8_t>(value >> 24));
        write(static_cast<uint8_t>((value >> 16) & 0xff));
        write(static_cast<uint8_t>((value >> 8) & 0xff));
        write(static_cast<uint8_t>(value & 0xff));
    }
}

void BinaryStream::write(const std::filesystem::path& filename)
{
    auto binary = Binary::load(filename);
    write(binary);
}

void BinaryStream::write(const Binary& binary)
{
    m_stream.write(reinterpret_cast<const char*>(binary.data()), binary.size());
}

void BinaryStream::write(const std::string& string)
{
    m_stream.write(string.data(), string.size());
}

void BinaryStream::write(uint8_t value, size_t count)
{
    for (auto i = 0U; i < count; ++i)
    {
        write(value);
    }
}

auto BinaryStream::readBinary(size_t length) -> Binary
{
    std::vector<uint8_t> data(length);
    m_stream.read(reinterpret_cast<char*>(data.data()), length);
    return Binary::fromArray(data.data(), length);
}

BinaryStream::operator bool() const
{
    return !m_stream.fail();
}

} // namespace kaizo