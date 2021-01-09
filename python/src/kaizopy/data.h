#pragma once

#include <pybind11/pybind11.h>

void registerKaizoData(pybind11::module_& m);
void registerKaizoDataFormats(pybind11::module_& m);
void registerKaizoDataLinking(pybind11::module_& m);
