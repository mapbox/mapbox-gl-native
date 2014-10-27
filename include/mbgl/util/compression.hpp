#ifndef MBGL_UTIL_COMPRESSION
#define MBGL_UTIL_COMPRESSION

#include <string>

namespace mbgl {
namespace util {

std::string compress(const std::string &raw);
std::string decompress(const std::string &raw);

}
}

#endif
