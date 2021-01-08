#include <fuse/utilities/DomReaderHelpers.h>

namespace kaizo::data::details {

template <> auto display<const std::string&>(const std::string& key) -> std::string
{
    return key;
}

template <> auto display<const char*>(const char* key) -> std::string
{
    return std::string{key};
}

} // namespace kaizo::data