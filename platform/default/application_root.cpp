#include <mbgl/platform/platform.hpp>

#include <mbgl/util/uv.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string applicationRoot() {
    return uv::cwd();
}

}
}
