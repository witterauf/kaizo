#pragma once

#include "Binary.h"
#include <filesystem>
#include <fstream>
#include <string>

namespace fuse {

class BinaryStream
{
public:
    enum class Endianness
    {
        Little,
        Big
    };

    enum class Mode
    {
        Input,
        Output,
        InputOutput
    };

    explicit BinaryStream(const std::filesystem::path& filename, Mode mode);

    void setEndianness(Endianness endianness);
    void setLittleEndian();
    void setBigEndian();

    void write(uint8_t value);
    void write(char value);
    void write(uint16_t value);
    void write(uint32_t value);
    void write(const std::filesystem::path& filename);
    void write(const Binary& binary);
    void write(const std::string& string);

    operator bool() const;

private:
    Endianness m_endianness{Endianness::Little};
    std::fstream m_stream;
};

} // namespace fuse