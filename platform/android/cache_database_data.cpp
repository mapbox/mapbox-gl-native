#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
// TODO need to use get cache dir from java
    return "/data/data/com.mapbox.mapboxgl.app/mbgl-cache.db";
}

}
}
