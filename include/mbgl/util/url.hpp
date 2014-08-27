#ifndef MBGL_UTIL_URL
#define MBGL_UTIL_URL

#include <string>

namespace mbgl {
namespace util {

std::string percentEncode(const std::string&);
std::string percentDecode(const std::string&);

}
}

#endif
