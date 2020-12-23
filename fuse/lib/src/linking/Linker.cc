#include "Linker.h"
#include "Backtracker.h"
#include "LinkLuaLibrary.h"
#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/LuaFuseLibrary.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/StringAlgorithms.h>
#include <regex>

namespace fuse {

Linker::Linker(const Arguments& arguments)
    : m_arguments{arguments}
{
    m_packer = std::make_unique<BacktrackingPacker>();
}

void Linker::run()
{
    initializeLua();
    if (m_arguments.doPacking)
    {
        loadObjects();
        loadConstraints();
        loadTargets();
        pack();
        if (m_arguments.targetAllocationFile)
        {
            writeAllocationFile();
        }
    }
    if (m_arguments.doLinking)
    {
        loadAllocationFiles();
        // resolveReferences();
        writeBinaries();
    }
}

void Linker::initializeLua()
{
    m_lua.open_libraries(sol::lib::base);
    openFuseLibrary(m_lua.lua_state());
    m_lua["link"] = openLinkLibrary(m_lua.lua_state());
    m_lua.new_usertype<Linker>("Linker", "addConstraint", &Linker::addConstraint, "addTarget",
                               &Linker::addTarget);
    m_lua["linker"] = this;
}

void Linker::addConstraint(const std::string& regexString, const Constraint& constraint)
{
    m_constraints.addConstraint(std::regex{regexString}, constraint.copy());

    if (shouldLogHierarchyLevel(0))
    {
        std::cout << "Added constraint /" << regexString << "/ -> " << constraint.toString()
                  << "\n";
    }
}

void Linker::addTarget(Target target)
{
    {
        auto const iter = m_arguments.targetPaths.find(target.id());
        if (iter == m_arguments.targetPaths.end())
        {
            throw std::runtime_error{"did not specify an input path for target '" + target.id() +
                                     "'"};
        }
        target.setPath(iter->second);
    }
    {
        auto const iter = m_arguments.targetOutputPaths.find(target.id());
        if (iter == m_arguments.targetOutputPaths.end())
        {
            throw std::runtime_error{"did not specify an output path for target '" + target.id() +
                                     "'"};
        }
        target.setOutputPath(iter->second);
    }

    m_targets.addTarget(target);

    if (shouldLogHierarchyLevel(0))
    {
        std::cout << "Added target " << target.path().string() << "\n";
        if (shouldLogHierarchyLevel(1))
        {
            for (auto i = 0U; i < target.freeBlockCount(); ++i)
            {
                std::cout << "  "
                          << "added free block " << target.freeBlock(i).toString() << "\n";
            }
        }
    }
}

void Linker::loadObjects()
{
    for (auto const& objectFile : m_arguments.objects)
    {
        auto result = m_lua.script_file(objectFile.string());
        if (!result.valid())
        {
            throw std::runtime_error{"Object file '" + objectFile.string() +
                                     "' did not return valid objects"};
        }
        else if (result.get_type() == sol::type::table)
        {
            LuaDomReader reader{result.get<sol::table>()};
            auto binary = AnnotatedBinary::deserialize(reader);
            addAnnotatedBinary(std::move(binary));
        }
    }
}

void Linker::addAnnotatedBinary(std::unique_ptr<AnnotatedBinary>&& binary)
{
    m_packedObjects.push_back(std::move(binary));

    if (shouldLogHierarchyLevel(0))
    {
        auto const* added = m_packedObjects.back().get();
        std::cout << "Added packed binary with " << added->objectCount() << " object(s)\n";
        if (shouldLogHierarchyLevel(1))
        {
            for (auto i = 0U; i < added->objectCount(); ++i)
            {
                auto const* object = added->object(i);
                std::cout << "  added object '" << object->path().toString() << "' with size "
                          << object->realSize() << ", " << object->sectionCount()
                          << " section(s) and " << object->unresolvedReferenceCount()
                          << " unresolved reference(s)\n";

                if (shouldLogHierarchyLevel(2))
                {
                    for (auto j = 0U; j < object->sectionCount(); ++j)
                    {
                        auto const& section = object->section(j);
                        std::cout << "    section @ " << section.offset
                                  << " (actual: " << section.realOffset << ") with size "
                                  << section.size << "\n";
                    }
                    for (auto j = 0U; j < object->unresolvedReferenceCount(); ++j)
                    {
                        auto const& reference = object->unresolvedReference(j);
                        std::cout << "    unresolved reference @ " << reference.relativeOffset()
                                  << " referencing '" << reference.referencedPath().toString()
                                  << "'\n";
                    }
                }
            }
        }
    }
}

void Linker::loadConstraints()
{
    sol::function luaLoadConstraints = m_lua["link"]["loadConstraints"];
    for (auto const& constraintFile : m_arguments.constraints)
    {
        auto result = m_lua.script_file(constraintFile.string());
        if (!result.valid())
        {
            throw std::runtime_error{"Constraint file '" + constraintFile.string() +
                                     "' did not return valid constraints"};
        }
        else if (result.get_type() == sol::type::table)
        {
            luaLoadConstraints(result.get<sol::table>());
        }
    }
}

void Linker::loadTargets()
{
    sol::function luaLoadTarget = m_lua["link"]["loadTargets"];
    for (auto const& targetFile : m_arguments.targets)
    {
        auto result = m_lua.script_file(targetFile.string());
        if (!result.valid())
        {
            throw std::runtime_error{"Constraint file '" + targetFile.string() +
                                     "' did not return valid free blocks"};
        }
        else if (result.get_type() == sol::type::table)
        {
            luaLoadTarget(result.get<sol::table>());
        }
    }
}

void Linker::pack()
{
    for (auto k = 0U; k < m_targets.targetCount(); ++k)
    {
        auto const& target = m_targets.target(k);
        for (auto i = 0U; i < target.freeBlockCount(); ++i)
        {
            m_packer->addFreeBlock(target.freeBlock(i));
        }
    }
    for (auto const& objects : m_packedObjects)
    {
        for (auto i = 0U; i < objects->objectCount(); ++i)
        {
            auto object = std::make_unique<LinkObject>(objects->object(i));
            m_constraints.applyConstraints(*object);
            m_packer->addObject(object.get());
            m_linkObjects.insert(std::make_pair(object->path(), std::move(object)));
        }
    }
    if (shouldLogProgress())
    {
        std::cout << "Packing objects into free blocks...\n";
    }
    if (!m_packer->pack())
    {
        throw FuseException{"could not pack objects into the free blocks"};
    }

    if (shouldLogProgress())
    {
        std::cout << "Finished packing.\n";
        if (shouldLogResult())
        {
            for (auto const& objectPair : m_linkObjects)
            {
                std::cout << "  allocated object '" << objectPair.second->path().toString()
                          << "' @ " << objectPair.second->address().toString() << "\n";
            }
        }
    }
}

void Linker::writeAllocationFile()
{
    Expects(m_arguments.targetAllocationFile.has_value());
    LuaWriter writer;
    writer.startTable();
    for (auto const& objectPair : m_linkObjects)
    {
        writer.startField(objectPair.first.toString())
            .writeInteger(objectPair.second->address().toInteger())
            .finishField();
    }
    writer.finishTable();
    writer.savePlain(*m_arguments.targetAllocationFile);
}

void Linker::loadAllocationFiles()
{
}

void Linker::resolveReferences()
{
    for (auto& binary : m_packedObjects)
    {
        for (auto i = 0U; i < binary->objectCount(); ++i)
        {
            auto* object = binary->object(i);
            for (auto k = 0U; k < object->unresolvedReferenceCount(); ++k)
            {
                if (auto maybeAddress =
                        resolveReference(object->unresolvedReference(k).referencedPath()))
                {
                    auto patches = object->solveReference(k, *maybeAddress);
                }
            }
        }
    }
}

auto Linker::resolveReference(const binary::DataPath& path) const -> std::optional<Address>
{
    auto objectIter = m_linkObjects.find(path);
    if (objectIter != m_linkObjects.cend())
    {
        return objectIter->second->address();
    }
    auto iter = m_additionalAddreses.find(path);
    if (iter != m_additionalAddreses.cend())
    {
        return iter->second;
    }
    return {};
}

void Linker::writeBinaries()
{
    for (auto const& target : m_targets)
    {
        std::filesystem::create_directories(target.outputPath().parent_path());
        std::filesystem::copy(target.path(), target.outputPath(),
                              std::filesystem::copy_options::overwrite_existing);
    }

    for (auto& objectPair : m_linkObjects)
    {
        auto& linkObject = *objectPair.second;
        auto& target = m_targets.target(linkObject.address());
        auto const offset = target.toOffset(linkObject.address());
        auto const& binaryObject = linkObject.object();
        std::fstream output{target.outputPath(),
                            std::fstream::binary | std::fstream::out | std::fstream::in};

        std::vector<BinaryPatch> objectPatches;
        for (auto k = 0U; k < binaryObject.unresolvedReferenceCount(); ++k)
        {
            if (auto maybeAddress =
                    resolveReference(binaryObject.unresolvedReference(k).referencedPath()))
            {
                auto const patches = linkObject.object().solveReference(k, *maybeAddress);
                objectPatches.insert(objectPatches.end(), patches.cbegin(), patches.cend());
            }
        }

        unsigned patchIndex{0};
        for (auto i = 0U; i < objectPair.second->object().sectionCount(); ++i)
        {
            auto const& section = linkObject.object().section(i);
            auto const sectionOffset = offset + section.realOffset;
            auto const sectionSize = section.size;
            auto const sectionEnd = sectionOffset + sectionSize;

            auto binary = linkObject.object().sectionBinary(i);

            if (!objectPatches.empty())
            {
                auto realOffset = [&](size_t index) {
                    return offset +
                           binaryObject.toRealOffset(objectPatches[index].relativeOffset());
                };
                for (; patchIndex < objectPatches.size() && realOffset(patchIndex) < sectionEnd;
                     ++patchIndex)
                {
                    auto& patch = objectPatches[patchIndex];
                    auto const offsetWithinSection = patch.relativeOffset() - section.offset;

                    // If a patch writes partial bytes, then we have to read in what the binary says
                    // originally.
                    if (!patch.usesOnlyFullBytes())
                    {
                        // prefill in to-be-patched memory with values from binary
                        auto const realOffsetValue = realOffset(patchIndex);
                        output.seekp(realOffsetValue);
                        for (auto j = 0U; j < patch.size(); ++j)
                        {
                            uint8_t value;
                            output.read(reinterpret_cast<char*>(&value), 1);
                            binary[offsetWithinSection + j] = value;
                        }
                    }

                    patch.setRelativeOffset(offsetWithinSection);
                    patch.apply(binary, 0);
                }
            }

            output.seekp(sectionOffset);
            output.write(reinterpret_cast<const char*>(binary.data()), sectionSize);
        }
        output.close();
    }
}

bool Linker::shouldLogHierarchyLevel(size_t level) const
{
    return m_arguments.verbosity > level;
}

bool Linker::shouldLogProgress() const
{
    return m_arguments.verbosity >= 1;
}

bool Linker::shouldLogResult() const
{
    return m_arguments.verbosity >= 2;
}

} // namespace fuse