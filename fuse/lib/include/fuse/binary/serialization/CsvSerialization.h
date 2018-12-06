#include <fuse/binary/data/Data.h>
#include <fuse/binary/serialization/Serialization.h>

namespace fuse {

class CsvSerialization : public DataSerialization
{
public:
    void serialize(const binary::Data& data, const std::filesystem::path& filename) override;
    auto deserialize(const std::filesystem::path& filename)
        -> std::unique_ptr<binary::Data> override;

private:
    void serializeData(const binary::Data& data);
    void serializeRecord(const binary::Data& data);
    void serializeArray(const binary::Data& data);
    void serializeInteger(const binary::Data& integer);
    void serializeString(const binary::Data& string);
    void outputRow(binary::DataType type, const std::string& value);

    std::ofstream* m_output;

    void process(binary::Data* data, size_t pathIndex);
    void processExisting(binary::Data* data, size_t pathIndex);
    void processArray(binary::Data* data, size_t pathIndex);
    void processRecord(binary::Data* data, size_t pathIndex);
    auto makeLeaf() -> std::unique_ptr<binary::Data>;
    auto makeIntegerLeaf() -> std::unique_ptr<binary::Data>;
    auto makeStringLeaf() -> std::unique_ptr<binary::Data>;
    auto makeNext(size_t pathIndex) -> std::unique_ptr<binary::Data>;

    binary::Data* m_root{nullptr};
    binary::DataPath m_currentPath;
    binary::DataType m_currentType;
    std::string m_currentValue;
};

} // namespace fuse