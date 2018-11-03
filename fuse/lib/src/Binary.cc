#include <fstream>
#include <fuse/Binary.h>

namespace fuse {

namespace fs = std::filesystem;

auto Binary::fromArray(const uint8_t* data, size_t size) -> Binary
{
    Binary binary;
    binary.m_data.resize(size);
    memcpy(binary.m_data.data(), data, size);
    return binary;
}

auto Binary::load(const std::filesystem::path& filename) -> Binary
{
    if (!fs::exists(filename))
    {
        throw std::runtime_error{"file does not exist: " + filename.string()};
    }

    std::ifstream input{filename, std::ifstream::binary};
    if (input.good())
    {
        auto const filesize = fs::file_size(filename);
        Binary binary;
        binary.m_data.resize(filesize);
        input.read(reinterpret_cast<char*>(binary.m_data.data()), filesize);
        return std::move(binary);
    }
    else
    {
        throw std::runtime_error{"could not open file " + filename.string()};
    }
}

auto Binary::size() const -> size_t
{
    return m_data.size();
}

auto Binary::data(size_t offset) const -> const uint8_t*
{
    return m_data.data() + offset;
}

auto Binary::begin() const -> const uint8_t*
{
    return m_data.data();
}

auto Binary::end() const -> const uint8_t*
{
    return m_data.data() + size();
}

auto Binary::operator[](size_t offset) const -> uint8_t
{
    return m_data[offset];
}

auto Binary::operator[](size_t offset) -> uint8_t&
{
    return m_data[offset];
}

bool Binary::operator==(const Binary& rhs) const
{
    return m_data == rhs.m_data;
}

auto Binary::read(size_t offset, size_t length) const -> Binary
{
    Binary binary;
    binary.m_data = std::vector<uint8_t>(m_data.begin() + offset, m_data.begin() + offset + length);
    return std::move(binary);
}

auto Binary::asVector() const -> std::vector<uint8_t>
{
    return m_data;
}

void Binary::save(const std::filesystem::path& filename) const
{
    std::ofstream output{filename, std::ofstream::binary};
    if (output.good())
    {
        output.write(reinterpret_cast<const char*>(m_data.data()), m_data.size());
    }
    else
    {
        throw std::runtime_error{"could not open file: " + filename.string()};
    }
}

void Binary::clear()
{
    m_data.clear();
}

void Binary::append(uint8_t value)
{
    m_data.push_back(value);
}

void Binary::append(char value)
{
    m_data.push_back(static_cast<uint8_t>(value));
}

auto Binary::operator+=(const Binary& rhs) -> Binary&
{
    m_data.insert(m_data.end(), rhs.m_data.cbegin(), rhs.m_data.cend());
    return *this;
}

auto operator+(Binary lhs, const Binary& rhs) -> Binary
{
    return lhs += rhs;
}

} // namespace fuse