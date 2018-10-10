#include <fstream>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/LuaWriter.h>
//#include <fuse/binary/ArrayFormat.h>
//#include <fuse/binary/PointerFormat.h>
//#include <fuse/binary/IntegerFormat.h>
#include <fuse/binary/RecordFormat.h>
#include <fuse/binary/StringData.h>
#include <fuse/binary/StringFormat.h>
#include <fuse/binary/BinaryFormat.h>
#include <fuse/text/TableDecoder.h>
#include <fuse/text/TableReader.h>

using namespace fuse::binary;
using namespace fuse::text;

int main(int, char* [])
{
    DataReader reader("c:\\build\\sobs\\sobs.gbc");

    TableReader tableReader;
    auto table = tableReader.read("c:\\build\\sobs\\sobs_table.txt");
    if (!table)
    {
        return -1;
    }

    auto binaryFormat = std::make_unique<BinaryFormat>();
    binaryFormat->setSizeProvider(std::make_unique<FixedSizedProvider>(0x10));
    auto stringFormat = std::make_unique<TableStringFormat>();
    stringFormat->addTable(std::move(*table));
    auto recordFormat = std::make_unique<RecordFormat>();
    recordFormat->append("data", std::move(binaryFormat));
    recordFormat->append("name", stringFormat->copy());
    recordFormat->append("line1", stringFormat->copy());
    recordFormat->append("line2", std::move(stringFormat));

    reader.setOffset(0xC0001);
    auto string = recordFormat->decode(reader);

    LuaWriter writer;
    auto lua = writer.write(*string);
    std::ofstream output{ "c:\\build\\script.txt" };
    output << lua;
    return 0;
}