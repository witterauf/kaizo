#include "MakeCollection.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

void MakeCollection::run()
{
    validateArguments();
    prepare();
    findFiles();
    generateImplementationFile();
    generateHeaderFile();
}

static bool isValidIdentifier(const std::string& identifier)
{
    const std::regex regex("[a-zA-Z][a-zA-Z0-9_]*");
    return std::regex_match(identifier, regex);
}

static bool isValidNamespace(const std::string& identifier)
{
    const std::regex regex("[a-zA-Z][a-zA-Z0-9_]*(\\:\\:[a-zA-Z][a-zA-Z0-9_]*)*");
    return std::regex_match(identifier, regex);
}

void MakeCollection::validateArguments()
{
    if (!isValidIdentifier(m_arguments.accessor))
    {
        throw std::runtime_error{"'" + m_arguments.accessor +
                                 "' is not a supported C++ identifier"};
    }
    if (m_arguments.nameSpace && !isValidNamespace(*m_arguments.nameSpace))
    {
        throw std::runtime_error{"'" + *m_arguments.nameSpace +
                                 "' is not a supported C++ namespace"};
    }
}

void MakeCollection::prepare()
{
    if (m_arguments.workingDirectory)
    {
        auto const& workingDirectory = *m_arguments.workingDirectory;
        if (!fs::exists(workingDirectory))
        {
            throw std::runtime_error{"directory '" + workingDirectory.string() +
                                     "' does not exist"};
        }
        fs::current_path(workingDirectory);
    }
    m_basePath = fs::current_path();
}

void MakeCollection::findFiles()
{
    for (auto const& input : m_arguments.inputs)
    {
        findFilesForInput(input);
    }
}

void MakeCollection::findFilesForInput(const std::string& input)
{
    // TODO: support wildcards
    fs::path inputPath{input};
    if (!fs::exists(inputPath))
    {
        throw std::runtime_error{"input '" + inputPath.string() + "' does not exist"};
    }
    collect(inputPath);
}

static auto toString(const fs::file_type& type) -> std::string
{
    switch (type)
    {
    case fs::file_type::directory: return "directory";
    case fs::file_type::regular: return "regular file";
    case fs::file_type::symlink: return "symbolic link";
    case fs::file_type::block: return "block file";
    case fs::file_type::character: return "character file";
    case fs::file_type::fifo: return "FIFO";
    case fs::file_type::junction: return "junction";
    case fs::file_type::none: return "none";
    case fs::file_type::socket: return "socket";
    default: return "unknown";
    }
}

void MakeCollection::collect(const std::filesystem::path& path)
{
    auto const status = fs::status(path);
    switch (status.type())
    {
    case fs::file_type::directory: collectDirectory(path); return;
    case fs::file_type::regular: collectFile(path); return;
    case fs::file_type::symlink: collectSymbolicLink(path); return;
    default: break;
    }
    std::cerr << "Warning: file type '" << toString(status.type()) << "' of '" << path.string()
              << "' is not supported";
}

void MakeCollection::collectFile(const fs::path& file)
{
    std::error_code ec;
    auto const relativePath = std::filesystem::relative(file, m_basePath, ec);
    if (!ec)
    {
        m_collectionFiles.push_back(
            {file, makeCollectionPath(relativePath), makeIdentifier(relativePath)});
    }
    else
    {
        throw std::runtime_error{ec.message()};
    }
}

auto MakeCollection::makeCollectionPath(const std::filesystem::path& path) -> std::string
{
    auto string = path.string();
    std::replace(string.begin(), string.end(), '\\', '/');
    return string;
}

void MakeCollection::collectDirectory(const fs::path& directory)
{
    for (auto& entry : std::filesystem::directory_iterator{directory})
    {
        collect(entry.path());
    }
}

void MakeCollection::collectSymbolicLink(const fs::path& symlink)
{
}

auto MakeCollection::makeIdentifier(const std::filesystem::path& path) -> std::string
{
    return "file_array_" + std::to_string(m_identifierCount++);
}

auto MakeCollection::headerFileName() const -> std::filesystem::path
{
    return m_arguments.outputDirectory / (m_arguments.collectionName + ".h");
}

auto MakeCollection::implementationFileName() const -> std::filesystem::path
{
    return m_arguments.outputDirectory / (m_arguments.collectionName + ".cc");
}

void MakeCollection::generateImplementationFile()
{
    m_output = std::ofstream{implementationFileName()};
    if (m_output.good())
    {
        m_output << "#include \"" << m_arguments.collectionName << ".h\"\n";
        m_output << "#include <map>\n\n";
        m_output << "#include <string>\n\n";
        m_output << "namespace {\n\n";
        generateFileArrays();
        generateFileMap();
        m_output << "} // anonymous namespace\n\n";
        generateAccessorFunction();
    }
    else
    {
        throw std::runtime_error{"could not open implementation file '" +
                                 implementationFileName().string() + "' for writing"};
    }
}

void MakeCollection::generateFileArrays()
{
    for (auto const& file : m_collectionFiles)
    {
        m_output << generateFileArray(file);
    }
}

auto MakeCollection::generateFileArray(const CollectionFile& file) const -> std::string
{
    std::ifstream input{file.path, std::ifstream::binary};
    if (input.bad())
    {
        throw std::runtime_error{"could not open file '" + file.path.string() + "' for reading"};
    }
    auto const size = std::filesystem::file_size(file.path);
    if (size >= m_arguments.maximumFileSize)
    {
        throw std::runtime_error{"file '" + file.path.string() + "' is too large (" +
                                 std::to_string(size) + " bytes)"};
    }
    auto buffer = std::make_unique<uint8_t[]>(size);
    input.read(reinterpret_cast<char*>(buffer.get()), size);

    std::string byteArray;
    byteArray += "const uint8_t " + file.identifier + "[] = {";
    for (auto i = 0U; i < size; ++i)
    {
        if (i % 16 == 0)
        {
            byteArray += "\n    ";
        }
        byteArray += std::to_string(buffer[i]);
        byteArray += ",";
    }
    byteArray += "\n};\n\n";
    return std::move(byteArray);
}

void MakeCollection::generateFileMap()
{
    m_output
        << "const std::map<std::string, std::pair<const uint8_t*, size_t>> CollectionMap = {\n";
    for (auto const& file : m_collectionFiles)
    {
        m_output << "   {\"" << file.collectionPath << "\", {" + file.identifier << ", "
                 << "sizeof(" << file.identifier << ")}},\n";
    }
    m_output << "};\n\n";
}

void MakeCollection::generateAccessorFunction()
{
    if (m_arguments.nameSpace)
    {
        m_output << "namespace " << *m_arguments.nameSpace << " {\n\n";
    }

    m_output << "auto " << m_arguments.accessor << "(const char* name) -> std::pair<const uint8_t*, size_t>\n";
    m_output << "{\n";
    m_output << "   auto const iter = CollectionMap.find(std::string{name});\n";
    m_output << "   if (iter != CollectionMap.end())\n";
    m_output << "   {\n";
    m_output << "       return iter->second;\n";
    m_output << "   }\n";
    m_output << "   else\n";
    m_output << "   {\n";
    m_output << "       return std::make_pair(nullptr, 0ULL);\n";
    m_output << "   }\n";
    m_output << "}\n";

    if (m_arguments.nameSpace)
    {
        m_output << "\n} // namespace " << *m_arguments.nameSpace;
    }
}

void MakeCollection::generateHeaderFile()
{
    std::ofstream header{headerFileName()};
    if (header.good())
    {
        header << "#pragma once\n\n";
        header << "#include <cstddef>\n";
        header << "#include <cstdint>\n";
        header << "#include <utility>\n\n";

        if (m_arguments.nameSpace)
        {
            header << "namespace " << *m_arguments.nameSpace << " {\n\n";
        }
        header << "auto " << m_arguments.accessor
               << "(const char* name) -> std::pair<const uint8_t*, size_t>;\n";
        if (m_arguments.nameSpace)
        {
            header << "\n}";
        }
    }
    else
    {
        throw std::runtime_error{"could not open header file for writing"};
    }
}