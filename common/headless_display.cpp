#include "headless_view.hpp"

#include <stdexcept>

namespace mbgl {

HeadlessDisplay::HeadlessDisplay() {
#if MBGL_USE_CGL
    // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
    // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFAOpenGLProfile,
        (CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
        kCGLPFAAccelerated,
        (CGLPixelFormatAttribute) 0
    };

    GLint num;
    CGLError error = CGLChoosePixelFormat(attributes, &pixelFormat, &num);
    if (error) {
        fprintf(stderr, "Error pixel format: %s\n", CGLErrorString(error));
        return;
    }
#endif

#if MBGL_USE_GLX
    x_display = XOpenDisplay(0);

    if (x_display == nullptr) {
        throw std::runtime_error("Failed to open X display");
    }

    static int pixelFormat[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        None
    };

    x_info = glXChooseVisual(x_display, DefaultScreen(x_display), pixelFormat);

    if (x_info == nullptr) {
        throw std::runtime_error("Error pixel format");
    }
#endif
}

HeadlessDisplay::~HeadlessDisplay() {
#if MBGL_USE_CGL
    CGLDestroyPixelFormat(pixelFormat);
#endif

#if MBGL_USE_GLX
    XFree(x_info);
    XCloseDisplay(x_display);
#endif
}

}

