#include <mbgl/gl/headless_backend.hpp>

#include <OpenGLES/EAGL.h>

#include <stdexcept>

namespace mbgl {

struct EAGLImpl : public HeadlessBackend::Impl {
    EAGLImpl() {
        glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (glContext == nil) {
            throw std::runtime_error("Error creating GL context object");
        }
        glContext.multiThreaded = YES;
    }

    // Required for ARC to deallocate correctly.
    ~EAGLImpl() final = default;

    void activateContext() final {
        [EAGLContext setCurrentContext:glContext];
    }

    void deactivateContext() final {
        [EAGLContext setCurrentContext:nil];
    }

private:
    EAGLContext* glContext = nullptr;
};

gl::ProcAddress HeadlessBackend::initializeExtension(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<gl::ProcAddress>(symbol);
}

void HeadlessBackend::createContext() {
    assert(!hasContext());
    impl = std::make_unique<EAGLImpl>();
}

} // namespace mbgl
