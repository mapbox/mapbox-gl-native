#ifndef MBGL_MAP_MODE
#define MBGL_MAP_MODE

#include <cstdint>

namespace mbgl {

enum class MapMode : uint8_t {
    Continuous, // continually updating map
    Still, // a once-off still image
};

// We can avoid redundant GL calls when it is known that the GL context is not
// being shared. In a shared GL context case, we need to make sure that the
// correct GL configurations are in use - they might have changed between render
// calls.
enum class GLContextMode : uint8_t {
    Unique,
    Shared,
};

} // namespace mbgl

#endif // MBGL_MAP_MODE
