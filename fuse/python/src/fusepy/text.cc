#include "text.h"
#include "fusepy.h"
#include <fuse/text/AsciiEncoding.h>
#include <fuse/text/TextEncoding.h>

using namespace fuse;
using namespace fuse::text;
namespace py = pybind11;

auto TextEncoding_decode(TextEncoding& encoding, py::buffer b, const size_t offset)
    -> std::pair<size_t, std::string>
{
    auto const view = requestReadOnly(b);
    return encoding.decode(view, offset);
}

void registerFuseText(py::module_& m)
{
    py::class_<TextEncoding, std::shared_ptr<TextEncoding>>(m, "_TextEncoding")
        .def("encode",
             [](TextEncoding& encoding, const std::string& text) { return encoding.encode(text); })
        .def("decode", &TextEncoding_decode)
        .def_property_readonly("supports_encoding",
                               [](const TextEncoding& encoding) { return encoding.canEncode(); })
        .def_property_readonly("supports_decoding",
                               [](const TextEncoding& encoding) { return encoding.canDecode(); });

    m.add_object("_AsciiEncoding", py::cast(static_cast<TextEncoding*>(new AsciiEncoding{}),
                                            py::return_value_policy::take_ownership));
}
