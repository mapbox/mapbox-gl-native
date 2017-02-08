#pragma once

#include <string>

namespace mbgl {
namespace util {

class utf8_to_utf16 {
public:
    static std::u16string convert(std::string const&);
};
    
class utf16_to_utf8 {
public:
    static std::string convert(std::u16string const&);
};

} // namespace util
} // namespace mbgl
