#include <mbgl/platform/platform.hpp>

#include <mbgl/util/uv.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &applicationRoot() {
    static const std::string root = uv::cwd();
    return root;
}
}
}
