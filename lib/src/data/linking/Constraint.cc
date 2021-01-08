#include "kaizo/data/linking/Constraint.h"

namespace kaizo::data {

FixedAddressConstraint::FixedAddressConstraint(Address address)
    : m_address{address}
{
}

auto FixedAddressConstraint::findAllocations(const FreeSpace&, size_t) const
    -> std::vector<Allocation>
{
    return {};
}

bool FixedAddressConstraint::hasAllocations(const FreeSpace&, size_t) const
{
    return false;
}

auto FixedAddressConstraint::yieldsFixedAddress() const -> std::optional<Address>
{
    return m_address;
}

auto FixedAddressConstraint::strength() const -> unsigned
{
    return 1000;
}

auto FixedAddressConstraint::copy() const -> std::unique_ptr<Constraint>
{
    return std::make_unique<FixedAddressConstraint>(m_address);
}

auto FixedAddressConstraint::toString() const -> std::string
{
    return "FixedAddress(" + m_address.toString() + ")";
}

AndConstraint::AndConstraint(std::vector<std::unique_ptr<Constraint>>&& constraints)
    : m_constraints{std::move(constraints)}
{
}

auto AndConstraint::findAllocations(const FreeSpace&, size_t) const -> std::vector<Allocation>
{
    return {};
}

bool AndConstraint::hasAllocations(const FreeSpace&, size_t) const
{
    return false;
}

auto AndConstraint::yieldsFixedAddress() const -> std::optional<Address>
{
    for (auto const& constraint : m_constraints)
    {
        if (auto maybeFixedAddress = constraint->yieldsFixedAddress())
        {
            return maybeFixedAddress;
        }
    }
    return {};
}

auto AndConstraint::strength() const -> unsigned
{
    return 1;
}

auto AndConstraint::copy() const -> std::unique_ptr<Constraint>
{
    std::vector<std::unique_ptr<Constraint>> constraints;
    for (auto const& constraint : m_constraints)
    {
        constraints.push_back(constraint->copy());
    }
    return std::make_unique<AndConstraint>(std::move(constraints));
}

auto AndConstraint::toString() const -> std::string
{
    std::string repr{"And("};
    for (auto i = 0U; i < m_constraints.size(); ++i)
    {
        if (i > 0)
        {
            repr += ",";
        }
        repr += m_constraints[i]->toString();
    }
    repr += ")";
    return repr;
}

} // namespace kaizo::data