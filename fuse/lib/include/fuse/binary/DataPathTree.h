#pragma once

#include "DataPath.h"
#include <memory>
#include <vector>

namespace fuse::binary {

template <class T> class DataPathTree
{
    friend class DataPathTreeIterator<T>;

public:
    auto root() -> DataPathTreeIterator<T>;
    auto end() -> DataPathTreeIterator<T>;

private:
    struct Node
    {
        DataPathElement pathElement;
        T value;
        Node* parent;
        std::vector<std::unique_ptr<Node>> children;
    };

    Node m_root;
};

template <class T> class DataPathTreeIterator
{
public:
    auto pathElement() const -> const DataPathElement&;
    auto child(const DataPathElement& element) const -> DataPathTreeIterator;
    auto parent() const -> const DataPathTreeIterator;
    auto operator*() const -> const T&;
    auto operator*() -> T&;
    auto operator-> () const -> const T*;
    auto operator-> () -> T*;

private:
    DataPathTree<T> m_tree;
    DataPathTree<T>::Node* m_node;
};

} // namespace fuse::binary