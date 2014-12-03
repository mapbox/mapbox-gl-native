#include <mbgl/platform/platform.hpp>
#include <mbgl/android/jni.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
    return mbgl::android::cachePath + "/mbgl-cache.db";
}

}
}
