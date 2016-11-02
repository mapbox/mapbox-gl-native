#pragma once

#if defined(__QT__)
    #define MBGL_USE_QT 1
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define MBGL_USE_EAGL 1
    #else
        #define MBGL_USE_CGL 1
    #endif
#elif defined(__OSMESA__)
    #define MBGL_USE_OSMESA 1
#else
    #define MBGL_USE_GLX 1
#endif
