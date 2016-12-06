#pragma once

#include <string>

namespace mbgl {

namespace util {

std::string percentEncode(const std::string&);
std::string percentDecode(const std::string&);
bool isURL(const std::string&);

} // namespace util
} // namespace mbgl
