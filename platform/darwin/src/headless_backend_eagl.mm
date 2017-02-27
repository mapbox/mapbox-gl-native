#include <mbgl/gl/headless_backend.hpp>

#include <OpenGLES/EAGL.h>

#include <stdexcept>

namespace mbgl {

struct EAGLImpl : public HeadlessBackend::Impl {
    EAGLImpl(EAGLContext* glContext_) : glContext(glContext_) {
        [reinterpret_cast<EAGLContext*>(glContext) retain];
        reinterpret_cast<EAGLContext*>(glContext).multiThreaded = YES;
    }

    ~EAGLImpl() {
        [glContext release];
    }

    void activateContext() {
        [EAGLContext setCurrentContext:glContext];
    }

    void deactivateContext() {
        [EAGLContext setCurrentContext:nil];
    }

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

bool HeadlessBackend::hasDisplay() {
    return true;
}

void HeadlessBackend::createContext() {
    EAGLContext* glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (glContext == nil) {
        throw std::runtime_error("Error creating GL context object");
    }

    impl.reset(new EAGLImpl(glContext));
}

} // namespace mbgl
