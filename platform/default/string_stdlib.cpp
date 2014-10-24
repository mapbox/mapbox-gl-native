#include <mbgl/platform/platform.hpp>

#include <locale>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string& string) {
    // TODO: Use a proper Unicode Special Casing-aware algorithm.
    const auto &convert = std::use_facet<std::ctype<char>>(std::locale());
    std::string converted = string;
    convert.toupper(&converted[0], &converted[0] + converted.size());
    return converted;
}

std::string lowercase(const std::string& string) {
    // TODO: Use a proper Unicode Special Casing-aware algorithm.
    const auto &convert = std::use_facet<std::ctype<char>>(std::locale());
    std::string converted = string;
    convert.tolower(&converted[0], &converted[0] + converted.size());
    return converted;
}

}
}
