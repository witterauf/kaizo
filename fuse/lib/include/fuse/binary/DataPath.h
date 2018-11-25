#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fuse::binary {

class DataPathElement
{
public:
    enum class Kind
    {
        Name,
        IndexWildcard,
        Index,
        Parent,
        Pointer
    };

    static auto makeName(const std::string& name) -> DataPathElement;
    static auto makeIndex(size_t index) -> DataPathElement;
    static auto makeIndexWildcard() -> DataPathElement;
    static auto makePointer() -> DataPathElement;

    auto kind() const -> Kind;
    bool isName() const;
    bool isNameExpression() const;
    bool isIndex() const;
    bool isIndexExpression() const;
    bool isStatic() const;
    bool isWildcard() const;

    auto name() const -> const std::string&;
    auto index() const -> size_t;
    auto toString() const -> std::string;

    bool operator<(const DataPathElement& element) const;

private:
    Kind m_kind;
    std::variant<std::string, size_t> m_value;
};

class DataPath
{
public:
    static auto fromString(const std::string) -> std::optional<DataPath>;

    void goUp();
    auto parent() const -> DataPath;
    auto operator/=(const DataPathElement& element) -> DataPath&;

    auto toString() const -> std::string;

    bool operator<(const DataPath& rhs) const;

private:
    std::vector<DataPathElement> m_elements;
};

} // namespace fuse::binary