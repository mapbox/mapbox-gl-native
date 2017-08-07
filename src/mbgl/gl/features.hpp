#pragma once

#if __APPLE__
    #define MBGL_HAS_BINARY_PROGRAMS 0
#else
    #define MBGL_HAS_BINARY_PROGRAMS 1
#endif
