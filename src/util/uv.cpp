#include <mbgl/util/uv.hpp>

#include <uv.h>

namespace uv {

std::string cwd() {
    size_t max = 0;
    std::string dir;
    do {
        max += 256;
        dir.resize(max);
        uv_cwd(const_cast<char *>(dir.data()), &max);
    } while (max == dir.size());
    dir.resize(max - 1);
    return dir;
}

}
