#pragma once

#include <kaizo/binary/BinaryView.h>
#include <pybind11/pybind11.h>

auto requestReadOnly(pybind11::buffer& b) -> kaizo::BinaryView;
auto requestWritable(pybind11::buffer& b) -> kaizo::MutableBinaryView;
