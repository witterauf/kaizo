#pragma once

#include "Constraint.h"
#include <kaizo/addresses/Address.h>
#include <memory>
#include <optional>
#include <vector>

namespace kaizo::data {

class FreeSpace;
class Object;

class LinkObject
{
public:
    explicit LinkObject(const std::string& id, const size_t size);

    auto id() const -> const std::string&;
    auto size() const -> size_t;

    void setAllocation(const Allocation& allocation);
    void unsetAllocation();
    bool hasAllocation() const;
    auto allocation() const -> const Allocation&;

    void setFixedAddress(const Address address);
    bool hasFixedAddress() const;
    void constrain(std::unique_ptr<Constraint>&& constraint);
    bool isConstrained() const;
    bool isUnconstrained() const;

    auto findAllocations(const FreeSpace& space) const -> std::vector<Allocation>;
    bool hasAllocations(const FreeSpace& space) const;
    auto measureSlack(const FreeSpace& space) const -> size_t;

private:
    std::string m_id;
    const size_t m_size;
    bool m_hasFixedAddress{false};
    std::optional<Allocation> m_allocation;
    std::unique_ptr<Constraint> m_constraint;
};

} // namespace kaizo::data