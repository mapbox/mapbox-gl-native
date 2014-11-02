#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
    return "/data/data/com.mapbox.mapboxgl.app/mbgl-cache.db";
}

}
}
