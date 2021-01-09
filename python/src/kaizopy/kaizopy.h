#pragma once

#include <pybind11/pybind11.h>

void registerKaizoAddresses(pybind11::module_& m);
void registerKaizoData(pybind11::module_& m);
void registerKaizoGraphics(pybind11::module_& m);
void registerKaizoText(pybind11::module_& m);
void registerKaizoSystems(pybind11::module_& m);
