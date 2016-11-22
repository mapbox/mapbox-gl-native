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

#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
#endif
};

} // namespace mbgl
