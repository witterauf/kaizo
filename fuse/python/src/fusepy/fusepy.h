#pragma once

#include <fuse/BinaryView.h>
#include <pybind11/pybind11.h>

auto requestReadOnly(pybind11::buffer& b) -> kaizo::data::BinaryView;
auto requestWritable(pybind11::buffer& b) -> kaizo::data::MutableBinaryView;
