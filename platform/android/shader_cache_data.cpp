#include <mbgl/platform/platform.hpp>
#include <mbgl/android/jni.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default shader cache on this system.
std::string defaultShaderCache() {
    return mbgl::android::cachePath + "/mbgl-shader-cache-";
}

}
}
