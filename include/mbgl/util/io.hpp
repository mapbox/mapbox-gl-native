#ifndef MBGL_UTIL_IO
#define MBGL_UTIL_IO

#include <string>

namespace mbgl {
namespace util {

void write_file(const std::string &filename, const std::string &data);
std::string read_file(const std::string &filename);

}
}

#endif
