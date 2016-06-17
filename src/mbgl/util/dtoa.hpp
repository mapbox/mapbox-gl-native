#pragma once

#include <string>

namespace mbgl {
namespace util {

char* dtoa(double value, char* buffer);
std::string dtoa(double value);

} // end namespace util
} // end namespace mbgl
