#include "kaizopy.h"
#include <optional>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(kaizopy, m)
{
    m.doc() = "ROM hacking tools";

    registerKaizoAddresses(m);
    registerKaizoData(m);
    registerKaizoGraphics(m);
    registerKaizoText(m);
    registerKaizoSystems(m);
}