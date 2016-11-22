#include <mbgl/gl/headless_display.hpp>

#include <OpenGL/OpenGL.h>

#include <stdexcept>
#include <string>

namespace mbgl {

class HeadlessDisplay::Impl {
public:
    Impl();
    ~Impl();
    CGLPixelFormatObj pixelFormat = nullptr;
};

HeadlessDisplay::Impl::Impl() {
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
}

HeadlessDisplay::Impl::~Impl() {
    CGLDestroyPixelFormat(pixelFormat);
}

template <>
CGLPixelFormatObj HeadlessDisplay::attribute() const {
    return impl->pixelFormat;
}

HeadlessDisplay::HeadlessDisplay()
    : impl(std::make_unique<Impl>()) {
}

HeadlessDisplay::~HeadlessDisplay() {
}

} // namespace mbgl
