#ifndef MBGL_UTIL_IO
#define MBGL_UTIL_IO

#include <string>
#include <stdexcept>

namespace mbgl {
namespace util {

struct IOException : std::runtime_error {
    inline IOException(int err, const char* msg) : std::runtime_error(msg), code(err) {
    }
    const int code = 0;
};

void write_file(const std::string &filename, const std::string &data);
std::string read_file(const std::string &filename);

void deleteFile(const std::string& filename);

}
}

#endif
