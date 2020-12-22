#pragma once

#include "kaizomodule.h"
#include "pyutilities.h"

class SystemsModule : public KaizoModule
{
public:
    bool initialize() override;
    auto createFunctions() -> std::vector<PyMethodDef> override;
};
