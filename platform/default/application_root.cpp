#include <mbgl/platform/platform.hpp>

#include <mbgl/util/uv.hpp>

namespace mbgl {
namespace platform {

// Returns the path the application root.
std::string applicationRoot() {
    static const std::string root = uv::cwd();
    return root;
}

}
}
