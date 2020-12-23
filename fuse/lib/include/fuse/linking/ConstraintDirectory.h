#pragma once

#include "Constraint.h"
#include <diagnostics/Contracts.h>
#include <fuse/binary/DataPath.h>
#include <memory>
#include <regex>

namespace fuse {

class LinkObject;

class ConstraintDirectory
{
public:
    void addConstraint(const std::regex& regex, std::unique_ptr<Constraint>&& constraint);
    void applyConstraints(LinkObject& object);

private:
    struct RegExConstraint
    {
        std::regex regex;
        std::unique_ptr<Constraint> constraint;
    };

    std::vector<RegExConstraint> m_constraints;
};

} // namespace fuse