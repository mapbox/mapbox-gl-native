#pragma once

#if __APPLE__
    #define MBGL_HAS_BINARY_PROGRAMS 0
#else
    // https://github.com/mapbox/mapbox-gl-native/issues/14294
    #define MBGL_HAS_BINARY_PROGRAMS 0
#endif
