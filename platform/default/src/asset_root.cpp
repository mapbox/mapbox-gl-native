#include <mbgl/platform/platform.hpp>

#include <uv.h>
#include <libgen.h>

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &assetRoot() {
    static const std::string root = []() -> std::string {
        size_t max = 0;
        std::string dir;
        do {
            // Gradually increase the length of the string in case the path was truncated.
            max += 256;
            dir.resize(max);
            uv_exepath(const_cast<char *>(dir.data()), &max);
        } while (max == dir.size());
        dir.resize(max - 1);
        dir = dirname(const_cast<char *>(dir.c_str()));
        return dir;
    }();
    return root;
}

}
}
