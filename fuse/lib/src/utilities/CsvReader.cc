#include <cctype>
#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/utilities/CsvReader.h>

namespace fuse {

CsvReader::CsvReader(const std::filesystem::path& filename)
{
    std::ifstream input{filename, std::ifstream::binary};
    if (input.good())
    {
        m_size = std::filesystem::file_size(filename);
        m_buffer = std::make_unique<unsigned char[]>(m_size);
        input.read(reinterpret_cast<char*>(m_buffer.get()), m_size);
        m_position = 0;
    }
    else
    {
        throw std::runtime_error{"CsvReader: could not open " + filename.string()};
    }
}

auto CsvReader::nextRow() -> std::optional<std::vector<std::string>>
{
    if (!hasNext())
    {
        return {};
    }
    std::vector<std::string> row;
    while (hasNext())
    {
        row.push_back(nextColumn());
        if (!hasNext())
        {
            return row;
        }
        else if (fetch() == '\n')
        {
            consume();
            return row;
        }
        else if (fetch() == '\r')
        {
            consume();
            if (fetch() == '\n')
            {
                consume();
            }
            return row;
        }
        else if (fetch() != ',')
        {
            throw std::runtime_error{"unexpected character: " + fetch()};
        }
        consume();
    }
    row.push_back("");
    return row;
}

auto CsvReader::nextColumn() -> std::string
{
    if (fetch() == '"')
    {
        return parseQuoted();
    }
    else
    {
        return parseNotQuoted();
    }
}

auto CsvReader::parseQuoted() -> std::string
{
    std::string column;
    consume();
    while (fetch() != '"')
    {
        column += fetch();
        consume();
        if (!hasNext())
        {
            throw std::runtime_error{"unexpected end of CSV file"};
        }
    }
    while (fetch() != ',' && fetch() != '\n' && fetch() != '\r' && hasNext())
    {
        consume();
    }
    return column;
}

auto CsvReader::parseNotQuoted() -> std::string
{
    std::string column;
    while (fetch() != ',' && fetch() != '\n' && fetch() != '\r' && hasNext())
    {
        column += fetch();
        consume();
        if (!hasNext())
        {
            throw std::runtime_error{"unexpected end of CSV file"};
        }
    }
    return column;
}

bool CsvReader::hasNext() const
{
    return m_position < m_size;
}

auto CsvReader::fetch() const -> char
{
    return m_buffer[m_position];
}

void CsvReader::consume()
{
    m_position += 1;
}

} // namespace fuse