#include "CLI11.hpp"
#include "MakeCollection.h"
#include <iostream>

using namespace std::string_literals;

int main(int argc, char* argv[])
{
    CLI::App app{"make-collection"};
    MakeCollection::Arguments arguments;
    app.add_option("input", arguments.inputs, "Files or directories to compile into collection")
        ->type_name("INPUT")
        ->required();
    app.add_option("-o,--output-directory", arguments.outputDirectory,
                   "Change output directory to DIR")
        ->type_name("DIR");
    app.add_option("-c,--collection", arguments.collectionName, "Generate NAME.cc and NAME.h")
        ->type_name("NAME")
        ->required();

    app.add_option("-e,--exclude", arguments.inputs, "Exclude files matching PATTERN")
        ->type_name("PATTERN")
        ->group("File selection");
    app.add_option("-d,--directory", arguments.workingDirectory, "Change directory to DIR")
        ->type_name("DIR")
        ->group("File selection");
    app.add_option("-m,--maximum-file-size", arguments.maximumFileSize,
                   "Abort if any file exceeds SIZE (default: "s +
                       std::to_string(MakeCollection::Arguments::DefaultMaximumSize()) + ")")
        ->type_name("SIZE")
        ->group("File selection");

    app.add_option("-n,--namespace", arguments.nameSpace,
                   "Change namespace the collection is put into to IDENTIFIER")
        ->type_name("IDENTIFIER")
        ->group("C++ output");
    app.add_option("-a,--accessor", arguments.accessor,
                   "Change name of accessor function to IDENTIFIER (default: "s +
                       MakeCollection::Arguments::DefaultAccessor() + ")")
        ->type_name("IDENTIFIER")
        ->group("C++ output");
    CLI11_PARSE(app, argc, argv);

    MakeCollection MakeCollection{arguments};
    try
    {
        MakeCollection.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return -1;
    }

    return 0;
}