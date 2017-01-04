#pragma once

#include <mbgl/gl/implementation.hpp>

#if MBGL_USE_CGL
#include <OpenGL/OpenGL.h>
#elif MBGL_USE_GLX
typedef struct _XDisplay Display;
typedef struct __GLXFBConfigRec* GLXFBConfig;
#endif

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
