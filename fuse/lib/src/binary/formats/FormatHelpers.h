#pragma once

#include <fuse/binary/data/Data.h>
#include <fuse/binary/DataPath.h>

namespace kaizo::data::binary {

void expectDataType(DataType type, const Data& data, const DataPath& path);

}