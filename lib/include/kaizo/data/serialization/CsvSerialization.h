#include <kaizo/data/data/Data.h>
#include <kaizo/data/serialization/Serialization.h>

namespace kaizo::data {

class CsvSerialization : public DataSerialization
{
public:
    void serialize(const Data& data, const std::filesystem::path& filename) override;
    auto deserialize(const std::filesystem::path& filename)
        -> std::unique_ptr<Data> override;

private:
    void decodePath(const std::string& path);
    void decodeType(const std::string& type);
    void decodeValue(std::string&& value);

    void serializeData(const Data& data);
    void serializeRecord(const Data& data);
    void serializeArray(const Data& data);
    void serializeInteger(const Data& integer);
    void serializeString(const Data& string);
    void outputRow(DataType type, const std::string& value);

    std::ofstream* m_output;

    void process(Data* data, size_t pathIndex);
    void processRoot();
    void processExisting(Data* data, size_t pathIndex);
    void processArray(Data* data, size_t pathIndex);
    void processRecord(Data* data, size_t pathIndex);
    auto makeLeaf() -> std::unique_ptr<Data>;
    auto makeIntegerLeaf() -> std::unique_ptr<Data>;
    auto makeStringLeaf() -> std::unique_ptr<Data>;
    auto makeReferenceLeaf() -> std::unique_ptr<Data>;
    auto makeNext(size_t pathIndex) -> std::unique_ptr<Data>;

    std::unique_ptr<Data> m_root;
    DataPath m_currentPath;
    DataType m_currentType;
    std::string m_currentValue;
};

} // namespace kaizo::data