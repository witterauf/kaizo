#include "ConstraintDirectory.h"
#include "LinkObject.h"

namespace fuse {

void ConstraintDirectory::addConstraint(const std::regex& regex,
                                        std::unique_ptr<Constraint>&& constraint)
{
    m_constraints.push_back({regex, std::move(constraint)});
}

void ConstraintDirectory::applyConstraints(LinkObject& object)
{
    // Find applying Constraints
    auto const& path = object.path().toString();
    std::vector<std::unique_ptr<Constraint>> objectConstraints;
    for (auto const& constraint : m_constraints)
    {
        if (std::regex_match(path, constraint.regex))
        {
            objectConstraints.push_back(constraint.constraint->copy());
        }
    }

    // Compose Constraints or set fixed Address
    if (!objectConstraints.empty())
    {
        std::unique_ptr<Constraint> constraint;
        if (objectConstraints.size() == 1)
        {
            constraint = std::move(objectConstraints.front());
        }
        else
        {
            constraint = std::make_unique<AndConstraint>(std::move(objectConstraints));
        }

        if (auto maybeFixedAddress = constraint->yieldsFixedAddress())
        {
            object.setFixedAddress(*maybeFixedAddress);
        }
        else
        {
            object.constrain(std::move(constraint));
        }
    }
}

} // namespace fuse