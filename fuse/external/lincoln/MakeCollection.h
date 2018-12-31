#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

class MakeCollection
{
public:
    enum class CxxStandard
    {
        Cxx98,
        Cxx03,
        Cxx11,
        Cxx14,
        Cxx17
    };

    struct Arguments
    {
        static constexpr auto DefaultMaximumSize() -> size_t
        {
            return 64 * 1024;
        }

        static constexpr auto DefaultAccessor() -> const char*
        {
            return "open";
        }

        std::vector<std::string> inputs;
        std::vector<std::string> excluded;
        std::filesystem::path outputDirectory;
        std::string collectionName;
        std::optional<std::string> nameSpace;
        size_t maximumFileSize{DefaultMaximumSize()};
        CxxStandard cxxStandard{CxxStandard::Cxx17};
        std::optional<std::filesystem::path> workingDirectory;
        std::string accessor{DefaultAccessor()};
    };

    explicit MakeCollection(const Arguments& arguments)
        : m_arguments{arguments}
    {
    }

    void run();

private:
    struct CollectionFile
    {
        std::filesystem::path path;
        std::string collectionPath;
        std::string identifier;
    };

    void prepare();
    void validateArguments();
    void findFiles();
    void findFilesForInput(const std::string& input);
    void collect(const std::filesystem::path& path);
    void collectFile(const std::filesystem::path& file);
    void collectDirectory(const std::filesystem::path& directory);
    void collectSymbolicLink(const std::filesystem::path& symlink);

    void generateImplementationFile();
    void generateFileArrays();
    auto generateFileArray(const CollectionFile& file) const -> std::string;
    void generateFileMap();
    void generateAccessorFunction();
    void generateHeaderFile();

    auto headerFileName() const -> std::filesystem::path;
    auto implementationFileName() const -> std::filesystem::path;
    auto makeIdentifier(const std::filesystem::path& path) -> std::string;
    auto makeCollectionPath(const std::filesystem::path& path) -> std::string;

    Arguments m_arguments;
    std::ofstream m_output;
    std::vector<CollectionFile> m_collectionFiles;
    size_t m_identifierCount{0};
    std::filesystem::path m_basePath;
};
