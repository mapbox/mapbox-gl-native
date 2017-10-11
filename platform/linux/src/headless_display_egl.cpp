#include <mbgl/gl/headless_display.hpp>
#include <mbgl/util/logging.hpp>
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

    const EGLint attribs[] = {
#if MBGL_USE_GLES2
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };

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
