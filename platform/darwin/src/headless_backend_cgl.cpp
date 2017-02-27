#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/headless_display.hpp>

#include <OpenGL/OpenGL.h>
#include <CoreFoundation/CoreFoundation.h>

#include <string>
#include <stdexcept>

namespace mbgl {

struct CGLImpl : public HeadlessBackend::Impl {
    CGLImpl(CGLContextObj glContext_) : glContext(glContext_) {
    }

    ~CGLImpl() {
        CGLDestroyContext(glContext);
    }

    void activateContext() final {
        CGLError error = CGLSetCurrentContext(glContext);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Switching OpenGL context failed:") +
                                     CGLErrorString(error) + "\n");
        }
    }

    void deactivateContext() final {
        CGLError error = CGLSetCurrentContext(nullptr);
        if (error != kCGLNoError) {
            throw std::runtime_error(std::string("Removing OpenGL context failed:") +
                                     CGLErrorString(error) + "\n");
        }
    }

    CGLContextObj glContext = nullptr;
};

gl::ProcAddress HeadlessBackend::initializeExtension(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<gl::ProcAddress>(symbol);
}

bool HeadlessBackend::hasDisplay() {
    if (!display) {
        display.reset(new HeadlessDisplay);
    }
    return bool(display);
}

void HeadlessBackend::createContext() {
    assert(!hasContext());

    CGLContextObj glContext = nullptr;
    CGLError error = CGLCreateContext(display->attribute<CGLPixelFormatObj>(), nullptr, &glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error creating GL context object:") +
                                 CGLErrorString(error) + "\n");
    }

    error = CGLEnable(glContext, kCGLCEMPEngine);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error enabling OpenGL multithreading:") +
                                 CGLErrorString(error) + "\n");
    }

    impl.reset(new CGLImpl(glContext));
}

} // namespace mbgl
