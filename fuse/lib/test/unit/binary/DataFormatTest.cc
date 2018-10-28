#include <fstream>
#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/LuaWriter.h>
#include <fuse/binary/PointerFormat.h>
//#include <fuse/binary/IntegerFormat.h>
#include <fuse/binary/BinaryFormat.h>
#include <fuse/binary/RecordFormat.h>
#include <fuse/binary/StringData.h>
#include <fuse/binary/StringFormat.h>
#include <fuse/systems/gbc/GameboyColorAddress.h>
#include <fuse/text/TableDecoder.h>
#include <fuse/text/TableReader.h>

using namespace fuse::binary;
using namespace fuse::text;
using namespace fuse::systems::gbc;

int main(int, char* [])
{
    DataReader reader("c:\\git\\sobs\\sobs.gbc");

    TableReader tableReader;
    auto table = tableReader.read("c:\\git\\sobs\\sobs_table.txt");
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
    auto pointerFormat = std::make_unique<AbsolutePointerFormat>();
    pointerFormat->setAddressFormat(std::make_unique<GameboyColorAddressFormat>());
    pointerFormat->setPointedFormat(std::move(recordFormat));
    auto arrayFormat = std::make_unique<ArrayFormat>();
    arrayFormat->setElementFormat(std::move(pointerFormat));
    arrayFormat->setSizeProvider(std::make_unique<FixedSizeProvider>(725));

    reader.setOffset(0xC8472);
    reader.enter(DataPathElement::makeName("root"));
    auto string = arrayFormat->decode(reader);

    LuaWriter writer;
    auto lua = writer.write(*string);
    std::ofstream output{"c:\\git\\script.txt"};
    output << LuaWriter::format(lua);
    return 0;
}