#pragma once

#include "ConstraintDirectory.h"
#include "FreeSpace.h"
#include "LinkObject.h"
#include "Packer.h"
#include "TargetMap.h"
#include <filesystem>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <sol.hpp>
#include <vector>

namespace fuse {

class Linker
{
public:
    struct Arguments
    {
        int verbosity{2};

        bool doPacking{true};
        std::vector<std::filesystem::path> targets;
        std::vector<std::filesystem::path> objects;
        std::vector<std::filesystem::path> constraints;
        std::optional<std::filesystem::path> targetAllocationFile;
        std::map<std::string, std::filesystem::path> targetPaths;
        std::map<std::string, std::filesystem::path> targetOutputPaths;

        bool doLinking{true};
        std::vector<std::filesystem::path> sourceAllocationFiles;
    };

    explicit Linker(const Arguments& arguments);

    void run();

    void addConstraint(const std::string& regex, const Constraint& constraint);
    void addTarget(Target target);
    void addAnnotatedBinary(std::unique_ptr<AnnotatedBinary>&& binary);

private:
    void initializeLua();
    void loadConstraints();
    void loadObjects();
    void loadTargets();
    void pack();
    void resolveReferences();
    void writeBinaries();
    void writeAllocationFile();
    void loadAllocationFiles();

    auto resolveReference(const binary::DataPath& path) const -> std::optional<Address>;

    bool shouldLogHierarchyLevel(size_t level) const;
    bool shouldLogProgress() const;
    bool shouldLogResult() const;

    sol::state m_lua;
    Arguments m_arguments;
    ConstraintDirectory m_constraints;
    std::unique_ptr<Packer> m_packer;
    TargetMap m_targets;
    std::vector<std::unique_ptr<AnnotatedBinary>> m_packedObjects;
    std::map<binary::DataPath, std::unique_ptr<LinkObject>> m_linkObjects;
    std::map<binary::DataPath, Address> m_additionalAddreses;
    std::vector<BinaryPatch> m_referencePatches;
};

} // namespace fuse