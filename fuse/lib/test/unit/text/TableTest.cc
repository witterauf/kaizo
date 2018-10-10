#include <fuse/text/Table.h>
#include <fuse/text/TableReader.h>

using namespace fuse::text;

int main(int, char* [])
{
    TableReader reader;
    if (auto maybeTable = reader.read("c:\\build\\sobs\\sobs_table.txt"))
    {
        return 0;
    }

    return 0;
}