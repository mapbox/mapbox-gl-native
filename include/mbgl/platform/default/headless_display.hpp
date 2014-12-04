#ifndef MBGL_COMMON_HEADLESS_DISPLAY
#define MBGL_COMMON_HEADLESS_DISPLAY

#include <mbgl/platform/headless_view.hpp>

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
    GLXFBConfig *fb_configs = nullptr;
#endif
};

}

#endif
