#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default shader cache on this system.
std::string defaultShaderCache() {
    return "/tmp/mbgl-shader-cache-";
}

}
}
