#pragma once

#include "Binary.h"
#include <filesystem>
#include <fstream>
#include <kaizo/binary/Integers.h>
#include <string>

namespace kaizo {

class BinaryStream
{
public:
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

    void seek(size_t offset);
    auto writeOffset() -> size_t;
    auto size() -> size_t;

    void write(uint8_t value);
    void write(char value);
    void write(uint16_t value);
    void write(uint32_t value);
    void write(const std::filesystem::path& filename);
    void write(const Binary& binary);
    void write(const std::string& string);
    void write(uint8_t value, size_t count);

    auto readBinary(size_t length) -> Binary;

    operator bool() const;

private:
    Mode m_mode;
    Endianness m_endianness{Endianness::Little};
    std::fstream m_stream;
};

} // namespace kaizo