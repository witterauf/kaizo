#pragma once

#include <cstddef>
#include <fuse/Binary.h>
#include <map>

namespace fuse::binary {

class Data;

class DataReader
{
public:
    DataReader(const std::filesystem::path& filename);
    ~DataReader();

    auto binary() const -> const Binary&;
    auto offset() const -> size_t;
    void advance(size_t size);
    void setOffset(size_t offset);

    void clearStorage();
    void store(const std::string& label, std::unique_ptr<Data>&& data);
    bool has(const std::string& label) const;
    auto load(const std::string& label) const -> const Data&;

private:
    size_t m_offset{0};
    Binary m_source;
    std::map<std::string, std::unique_ptr<Data>> m_storage;
};

} // namespace fuse::binary