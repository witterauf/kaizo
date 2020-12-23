#include "Linker.h"
#include <CLI11.hpp>
#include <cstdlib>
#include <iostream>

using namespace fuse;

struct Arguments
{
    int verbosity{2};

    bool doPacking{true};
    std::vector<std::filesystem::path> targets;
    std::vector<std::filesystem::path> objects;
    std::vector<std::filesystem::path> constraints;
    std::optional<std::filesystem::path> targetAllocationFile;
    std::vector<std::string> targetPaths;
    std::vector<std::string> targetOutputPaths;

    bool doLinking{true};
    std::vector<std::filesystem::path> sourceAllocationFiles;
};

void setupCommandLine(CLI::App& app, Arguments& arguments)
{
    app.add_subcommand("pack", "Pack a scenario into a SCE file");

    // logging options
    app.add_option("-v,--verbosity", arguments.verbosity, "Changes the amount of logging")
        ->group("Logging");

    // packing options
    app.add_option("--constraints", arguments.constraints, "Adds constraints from LUAFILE")
        ->check(CLI::ExistingFile)
        ->type_name("LUAFILE")
        ->group("Packing");
    app.add_option("-i,--inputs", arguments.objects, "Adds objects from LUAFILE")
        ->check(CLI::ExistingFile)
        ->type_name("LUAFILE")
        ->group("Packing");
    app.add_option("-t,--targets", arguments.targets, "Adds targets from LUAFILE")
        ->check(CLI::ExistingFile)
        ->type_name("LUAFILE")
        ->group("Packing");
    app.add_option("-o,--output", arguments.targetAllocationFile,
                   "Writes allocated addresses to FILE")
        ->type_name("FILE")
        ->group("Packing");
    auto* onlyPack =
        app.add_flag("--only-pack", [&arguments](auto count) { arguments.doLinking = count == 0; },
                     "Performs only packing phase")
            ->group("Packing");

    // target options
    app.add_option("--target-input", arguments.targetPaths, "Specifies input PATH for a TARGET")
        ->type_name("TARGET=PATH")
        ->group("Targets");
    app.add_option("--target-output", arguments.targetOutputPaths,
                   "Specifies output PATH for a TARGET")
        ->type_name("TARGET=PATH")
        ->group("Targets");

    // linking options
    auto* allocations = app.add_option("--allocations", arguments.sourceAllocationFiles,
                                       "Adds allocations from LUAFILE")
                            ->check(CLI::ExistingFile)
                            ->type_name("LUAFILE")
                            ->group("Packing");
    app.add_flag("--only-link", [&arguments](auto count) { arguments.doPacking = count == 0; },
                 "Performs only linking phase")
        ->needs(allocations)
        ->excludes(onlyPack)
        ->group("Linking");
}

static auto unescape(const std::string& path) -> std::filesystem::path
{
    if (path.length() == 0)
    {
        return {};
    }
    if (path.front() == '"')
    {
        if (path.back() == '"')
        {
            return path.substr(1, path.length() - 2);
        }
        else
        {
            throw std::runtime_error{"incorrectly quoted path"};
        }
    }
    else if (path.front() == '\'')
    {
        if (path.back() == '\'')
        {
            return path.substr(1, path.length() - 2);
        }
        else
        {
            throw std::runtime_error{ "incorrectly quoted path" };
        }
    }
    return path;
}

static auto convert(const std::vector<std::string>& mappings)
    -> std::map<std::string, std::filesystem::path>
{
    std::map<std::string, std::filesystem::path> paths;
    for (auto const& mapping : mappings)
    {
        if (mapping.find('=', 0) != std::string::npos)
        {
            const auto target = mapping.substr(0, mapping.find('=', 0));
            const auto path = unescape(mapping.substr(mapping.find('=', 0) + 1));
            paths.insert(std::make_pair(target, path));
        }
        else
        {
            throw std::runtime_error{"format required is TARGET=PATH"};
        }
    }
    return paths;
}

static auto validate(const Arguments& arguments) -> Linker::Arguments
{
    Linker::Arguments linker;
    linker.verbosity = arguments.verbosity;
    linker.targets = arguments.targets;
    linker.objects = arguments.objects;
    linker.constraints = arguments.constraints;
    linker.targetAllocationFile = arguments.targetAllocationFile;
    linker.doLinking = arguments.doLinking;
    linker.sourceAllocationFiles = arguments.sourceAllocationFiles;
    linker.targetPaths = convert(arguments.targetPaths);
    linker.targetOutputPaths = convert(arguments.targetOutputPaths);
    return linker;
}

int main(int argc, char* argv[])
{
    Arguments arguments;
    CLI::App app{"tales-link"};
    setupCommandLine(app, arguments);
    CLI11_PARSE(app, argc, argv);
    try
    {
        Linker linker{validate(arguments)};
        linker.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Fatal error: (no message)\n";
    }
    return EXIT_SUCCESS;
}