#include <mbgl/gl/headless_display.hpp>

#include <GL/glx.h>

#include <cstring>
#include <stdexcept>
#include <string>

namespace mbgl {

class HeadlessDisplay::Impl {
public:
    Impl();
    ~Impl();

    Display* xDisplay = nullptr;
    GLXFBConfig* fbConfigs = nullptr;
};

HeadlessDisplay::Impl::Impl() {
    if (!XInitThreads()) {
        throw std::runtime_error("Failed to XInitThreads.");
    }

    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay == nullptr) {
        throw std::runtime_error("Failed to open X display.");
    }

    const auto *extensions = reinterpret_cast<const char *>(glXQueryServerString(xDisplay, DefaultScreen(xDisplay), GLX_EXTENSIONS));
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
}

HeadlessDisplay::Impl::~Impl() {
    XFree(fbConfigs);
    XCloseDisplay(xDisplay);
}

template <>
Display* HeadlessDisplay::attribute() const {
    return impl->xDisplay;
}

template <>
GLXFBConfig* HeadlessDisplay::attribute() const {
    return impl->fbConfigs;
}

HeadlessDisplay::HeadlessDisplay()
    : impl(std::make_unique<Impl>()) {
}

HeadlessDisplay::~HeadlessDisplay() = default;

} // namespace mbgl
