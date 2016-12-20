#pragma once

#include <string>

namespace mbgl {
namespace util {

class utf8_to_utf16 {
public:
    static std::u16string convert(std::string const&);
};

} // namespace util
} // namespace mbgl
