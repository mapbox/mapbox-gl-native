#ifndef MBGL_MAP_MODE
#define MBGL_MAP_MODE

#include <cstdint>

namespace mbgl {

enum class MapMode : uint8_t {
    Continuous, // continually updating map
    Still, // a once-off still image
};

}

#endif