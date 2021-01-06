#pragma once

#include <fuse/BinaryView.h>
#include <pybind11/pybind11.h>

auto requestReadOnly(pybind11::buffer& b) -> fuse::BinaryView;
auto requestWritable(pybind11::buffer& b) -> fuse::MutableBinaryView;
