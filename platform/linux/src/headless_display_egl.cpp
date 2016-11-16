#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>

#include <EGL/egl.h>

namespace mbgl {

class HeadlessDisplay::Impl {
public:
    Impl();
    ~Impl();

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig config = 0;
};

HeadlessDisplay::Impl::Impl() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to obtain a valid EGL display.\n");
    }

    EGLint major, minor, numConfigs;
    if (!eglInitialize(display, &major, &minor)) {
        throw std::runtime_error("eglInitialize() failed.\n");
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglBindAPI(EGL_OPENGL_ES_API) returned error %d", eglGetError());
        throw std::runtime_error("eglBindAPI() failed");
    }

#if !defined(__ANDROID__)
    // This shouldn't matter as we're rendering to a framebuffer.
    const EGLint attribs[] = { EGL_NONE };
#else
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE
    };
#endif // __ANDROID__

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs) || numConfigs != 1) {
        throw std::runtime_error("Failed to choose ARGB config.\n");
    }
}

HeadlessDisplay::Impl::~Impl() {
    eglTerminate(display);
}

template <>
EGLDisplay HeadlessDisplay::attribute() const {
    return impl->display;
}

template <>
EGLConfig& HeadlessDisplay::attribute() const {
    return impl->config;
}

HeadlessDisplay::HeadlessDisplay()
    : impl(std::make_unique<Impl>()) {
}

HeadlessDisplay::~HeadlessDisplay() {
}

} // namespace mbgl
