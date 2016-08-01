#include <mbgl/platform/default/headless_display.hpp>

#include <cstring>
#include <stdexcept>

#if MBGL_USE_EGL
#include <EGL/egl.h>
#include <fcntl.h>
#include <gbm.h>
#include <unistd.h>
#endif

#if MBGL_USE_GLX
#include <GL/glx.h>
#endif

namespace mbgl {

HeadlessDisplay::HeadlessDisplay() {
#if MBGL_USE_CGL
    // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
    // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFAOpenGLProfile,
        static_cast<CGLPixelFormatAttribute>(kCGLOGLPVersion_Legacy),
        static_cast<CGLPixelFormatAttribute>(0)
    };

    GLint num;
    CGLError error = CGLChoosePixelFormat(attributes, &pixelFormat, &num);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error choosing pixel format:") + CGLErrorString(error) + "\n");
    }
    if (num <= 0) {
        throw std::runtime_error("No pixel formats found.");
    }
#endif

#if MBGL_USE_EGL
    // TODO(vignatti): it should search for a valid render* handle instead.
    const char device_name[] = "/dev/dri/renderD128";
    fd = open(device_name, O_RDWR);
    if (fd < 0) {
        throw std::runtime_error("Couldn't open drm device.");
    }

    gbm = gbm_create_device(fd);
    if (gbm == NULL) {
        throw std::runtime_error("Couldn't create gbm device.");
    }

    dpy = eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(gbm));
    if (dpy == EGL_NO_DISPLAY) {
        throw std::runtime_error("eglGetDisplay() failed.");
    }

    EGLint major, minor, n;
    if (!eglInitialize(dpy, &major, &minor)) {
        throw std::runtime_error("eglInitialize() failed.");
    }

    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };
    if (!eglChooseConfig(dpy, attribs, &config, 1, &n) || n != 1) {
        throw std::runtime_error("Failed to choose argb config.");
    }
#endif

#if MBGL_USE_GLX
    if (!XInitThreads()) {
        throw std::runtime_error("Failed to XInitThreads.");
    }

    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay == nullptr) {
        throw std::runtime_error("Failed to open X display.");
    }

    const char *extensions = reinterpret_cast<const char *>(glXQueryServerString(xDisplay, DefaultScreen(xDisplay), GLX_EXTENSIONS));
    if (!extensions) {
        throw std::runtime_error("Cannot read GLX extensions.");
    }
    if (!strstr(extensions,"GLX_SGIX_fbconfig")) {
        throw std::runtime_error("Extension GLX_SGIX_fbconfig was not found.");
    }
    if (!strstr(extensions, "GLX_SGIX_pbuffer")) {
        throw std::runtime_error("Cannot find glXCreateContextAttribsARB.");
    }

    // We're creating a dummy pbuffer anyway that we're not using.
    static int pixelFormat[] = {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        None
    };

    int configs = 0;
    fbConfigs = glXChooseFBConfig(xDisplay, DefaultScreen(xDisplay), pixelFormat, &configs);
    if (fbConfigs == nullptr) {
        throw std::runtime_error("Failed to glXChooseFBConfig.");
    }
    if (configs <= 0) {
        throw std::runtime_error("No Framebuffer configurations.");
    }
#endif
}

HeadlessDisplay::~HeadlessDisplay() {
#if MBGL_USE_CGL
    CGLDestroyPixelFormat(pixelFormat);
#endif

#if MBGL_USE_EGL
    eglTerminate(dpy);
    gbm_device_destroy(gbm);
    close(fd);
#endif
#if MBGL_USE_GLX
    XFree(fbConfigs);
    XCloseDisplay(xDisplay);
#endif
}

} // namespace mbgl
