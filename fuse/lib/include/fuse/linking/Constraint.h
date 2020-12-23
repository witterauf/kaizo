#pragma once

#include <cstddef>
#include <fuse/addresses/Address.h>
#include <memory>
#include <vector>

namespace fuse {

class FreeSpace;

/// Describes a range where a LinkObject can be allocated while
/// not violating the constraint
/// -> Intersection
struct AllocationCandidate
{
    size_t block;
    Address start;
    size_t size;
};

class Constraint
{
public:
    /// Finds all viable allocations for this constraint with the given size.
    /// Also used for computing the slack?
    virtual auto findAllocations(const FreeSpace& space, size_t) const
        -> std::vector<AllocationCandidate> = 0;

    /// Checks whether the FreeSpace can still satisfy this constraint.
    virtual bool hasAllocations(const FreeSpace& space, size_t) const = 0;

    /// Checks if the constraint can only result in a single, fixed Address.
    virtual auto yieldsFixedAddress() const -> std::optional<Address> = 0;

    /// The stronger a constraint, the earlier it is called to supply free blocks.
    /// This serves to minimize the number of constraint checks.
    virtual auto strength() const -> unsigned = 0;

    virtual auto copy() const -> std::unique_ptr<Constraint> = 0;
    virtual auto toString() const -> std::string = 0;
};

class FixedAddressConstraint : public Constraint
{
public:
    explicit FixedAddressConstraint(Address address);

    auto findAllocations(const FreeSpace& space, size_t) const
        -> std::vector<AllocationCandidate> override;
    bool hasAllocations(const FreeSpace& space, size_t) const override;
    auto yieldsFixedAddress() const -> std::optional<Address> override;
    auto strength() const -> unsigned override;
    auto copy() const -> std::unique_ptr<Constraint> override;
    auto toString() const -> std::string override;

private:
    Address m_address;
};

class AndConstraint : public Constraint
{
public:
    explicit AndConstraint(std::vector<std::unique_ptr<Constraint>>&& constraints);

    auto findAllocations(const FreeSpace& space, size_t) const
        -> std::vector<AllocationCandidate> override;
    bool hasAllocations(const FreeSpace& space, size_t) const override;
    auto yieldsFixedAddress() const -> std::optional<Address> override;
    auto strength() const -> unsigned override;
    auto copy() const -> std::unique_ptr<Constraint> override;
    auto toString() const -> std::string override;

private:
    std::vector<std::unique_ptr<Constraint>> m_constraints;
};

// Constraint notes
// ================
//
// isolated:
// - FixedAddress => findBlockThatContains()
// - FixedAddressRange => findBlocksWithinRange()
// - FixedSegment => findBlocksWithSegment()
// - FixedAttribute => findBlocksWithAttribute()
// - Alignment
// - Arbitrary boolean function on FreeBlock?
//   => can only loop over all blocks to find match
//   => might need to also supply slack() function
//   => intersect with other constraints if possible to reduce search space
//
// FixedAddress subsumes ALL other constraints
//
// transitive:
// - SameSegment : objects that must live in the same segment
//                 (represents jumps/loads within the same segment)
//   first object: degenerates to no constraint?
//   from second object: FixedSegment?
// - RelativeRange : object A must have at most a certain distance to object B
//              (represents relative jumps, loads, etc.)
//   first object: no constraint, -> isolated constraint
//   from second object: FixedAddressRange?
//   problem: distance chains
// - what about SameSegment & Distance (relative within same segment)?
// - all degenerate into isolated constraints after first allocation?
//   RelativeRange constraints form a chain...
//
// ConstraintGroup: put all objects with relative constraints into a group
//   if, for example, one object has a FixedAddress/FixedSegment, then all
//   objects with a SameSegment constraint must be in that segment
//
// major problems:
// - how to find feasible blocks for the constraint(s)?
//   idea: combine constraints by constraint subsumption, otherwise intersect
//         found blocks from all constraints?
//         constraint strength/priority -> intersection
//   idea: cache blocks also according to segment, attributes, etc
//         (then what about splitting?)
//
// => need for Constraint "algebra"?
//

} // namespace fuse