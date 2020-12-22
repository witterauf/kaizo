#include "kaizomodule.h"

bool KaizoModule::initialize()
{
    return true;
}

auto KaizoModule::createTypes() -> std::map<std::string, PyTypeObject*>
{
    return {};
}

auto KaizoModule::createFunctions() -> std::vector<PyMethodDef>
{
    return {};
}
