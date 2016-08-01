#pragma once

#include <mbgl/platform/default/headless_view.hpp>

namespace mbgl {

class HeadlessDisplay {
public:
    HeadlessDisplay();
    ~HeadlessDisplay();

#if MBGL_USE_CGL
    CGLPixelFormatObj pixelFormat = nullptr;
#endif

#if MBGL_USE_EGL
    int fd;
    struct gbm_device *gbm = nullptr;
    EGLDisplay dpy = 0;
    EGLConfig config = 0;
#endif
#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
#endif
};

} // namespace mbgl
