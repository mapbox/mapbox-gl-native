#pragma once

#include <string>

namespace mbgl {
namespace util {

std::u16string convertUTF8ToUTF16(const std::string&);
std::string convertUTF16ToUTF8(const std::u16string&);

} // namespace util
} // namespace mbgl
