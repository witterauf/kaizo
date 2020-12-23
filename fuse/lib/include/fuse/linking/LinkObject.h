#pragma once

#include "Constraint.h"
#include <fuse/binary/DataPath.h>
#include <memory>
#include <optional>
#include <vector>

namespace fuse {

class FreeSpace;
class Object;

class LinkObject
{
public:
    explicit LinkObject(const Object* object);

    auto size() const -> size_t;
    auto path() const -> const binary::DataPath&;
    auto object() const -> const Object&;

    void setAddress(const Address address);
    void unsetAddress();
    bool hasAddress() const;
    auto address() const -> const Address&;

    void setFixedAddress(const Address address);
    bool hasFixedAddress() const;

    auto findAllocations(const FreeSpace& space) const -> std::vector<AllocationCandidate>;
    bool hasAllocations(const FreeSpace& space) const;
    auto measureSlack(const FreeSpace& space) const -> size_t;

    void constrain(std::unique_ptr<Constraint>&& constraint);
    bool isConstrained() const;
    bool isUnconstrained() const;

private:
    const Object* m_object;
    std::unique_ptr<Constraint> m_constraint;
    bool m_hasFixedAddress{false};
    std::optional<Address> m_address;
};

} // namespace fuse