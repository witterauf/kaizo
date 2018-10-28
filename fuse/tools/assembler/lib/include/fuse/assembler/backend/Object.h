#pragma once

#include "Constraint.h"
#include "Reference.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace fuse::assembler {

class Object
{
public:
    void addConstraint(std::unique_ptr<Constraint>&& constraint);
    auto constraintCount() const -> size_t;
    auto constraint(size_t index) const -> const Constraint&;

    void addReference(std::unique_ptr<Reference>&& reference);
    auto referenceCount() const -> size_t;
    auto reference(size_t index) const -> const Reference&;

    auto binarySize() const -> size_t;
    void appendBinary(const uint8_t* binary, size_t bytes);

    template <class T> void append(T value)
    {
        appendBinary(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

private:
    std::vector<std::unique_ptr<Constraint>> m_constraints;
    std::vector<std::unique_ptr<Reference>> m_references;
    std::vector<uint8_t> m_binary;
};

} // namespace fuse::assembler