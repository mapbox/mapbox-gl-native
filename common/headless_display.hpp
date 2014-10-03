#ifndef MBGL_COMMON_HEADLESS_DISPLAY
#define MBGL_COMMON_HEADLESS_DISPLAY

#ifdef __APPLE__
#define MBGL_USE_CGL 1
#else
#include <GL/glx.h>
#define MBGL_USE_GLX 1
#endif

#include <mbgl/platform/gl.hpp>

namespace mbgl {

class HeadlessDisplay {
public:
    HeadlessDisplay();
    ~HeadlessDisplay();

#if MBGL_USE_CGL
    CGLPixelFormatObj pixelFormat;
#endif

#if MBGL_USE_GLX
    Display *x_display = nullptr;
    XVisualInfo *x_info = nullptr;
#endif
};

}

#endif
