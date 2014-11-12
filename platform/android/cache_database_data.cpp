#include <mbgl/platform/platform.hpp>
#include <mbgl/android/jni.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
// TODO need to use get cache dir from java
    return mbgl::android::cache_path + "/mbgl-cache.db";
}

}
}
