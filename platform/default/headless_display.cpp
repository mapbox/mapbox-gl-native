#include <mbgl/platform/default/headless_display.hpp>

#include <cstring>
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
    if (!XInitThreads()) {
        throw std::runtime_error("Failed to XInitThreads");
    }

    x_display = XOpenDisplay(nullptr);
    if (x_display == nullptr) {
        throw std::runtime_error("Failed to open X display");
    }

    const char *extensions = (char *)glXQueryServerString(x_display, DefaultScreen(x_display), GLX_EXTENSIONS);
    if (!extensions) {
        throw std::runtime_error("Cannot read GLX extensions");
    }
    if (!strstr(extensions,"GLX_SGIX_fbconfig")) {
        throw std::runtime_error("Extension GLX_SGIX_fbconfig was not found");
    }
    if (!strstr(extensions, "GLX_SGIX_pbuffer")) {
        throw std::runtime_error("Cannot find glXCreateContextAttribsARB");
    }

    // We're creating a dummy pbuffer anyway that we're not using.
    static int pixelFormat[] = {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        None
    };

    int configs = 0;
    fb_configs = glXChooseFBConfig(x_display, DefaultScreen(x_display), pixelFormat, &configs);
    if (configs <= 0) {
        throw std::runtime_error("No Framebuffer configurations");
    }
#endif
}

HeadlessDisplay::~HeadlessDisplay() {
#if MBGL_USE_CGL
    CGLDestroyPixelFormat(pixelFormat);
#endif

#if MBGL_USE_GLX
    XFree(fb_configs);
    XCloseDisplay(x_display);
#endif
}

}

