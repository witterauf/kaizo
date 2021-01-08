#pragma once

#include <kaizo/data/data/Data.h>
#include <kaizo/data/DataPath.h>

namespace kaizo::data {

void expectDataType(DataType type, const Data& data, const DataPath& path);

}