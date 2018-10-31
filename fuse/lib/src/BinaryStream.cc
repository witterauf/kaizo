#include <fuse/BinaryStream.h>
#include <diagnostics/Contracts.h>

namespace fuse {

BinaryStream::BinaryStream(const std::filesystem::path& filename, Mode mode)
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
    default:
        InvalidCase(mode);
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

BinaryStream::operator bool() const
{
    return !m_stream.fail();
}

} // namespace fuse