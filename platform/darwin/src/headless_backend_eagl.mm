#include <mbgl/platform/default/headless_backend.hpp>

#include <mbgl/gl/extension.hpp>

#include <OpenGLES/EAGL.h>

#include <stdexcept>

namespace mbgl {

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<gl::glProc>(symbol);
}

void HeadlessBackend::createContext() {
    glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (glContext == nil) {
        throw std::runtime_error("Error creating GL context object");
    }
    [reinterpret_cast<EAGLContext*>(glContext) retain];
    reinterpret_cast<EAGLContext*>(glContext).multiThreaded = YES;
}

void HeadlessBackend::destroyContext() {
    [reinterpret_cast<EAGLContext*>(glContext) release];
    glContext = nil;
}

void HeadlessBackend::activateContext() {
    [EAGLContext setCurrentContext:reinterpret_cast<EAGLContext*>(glContext)];
}

void HeadlessBackend::deactivateContext() {
    [EAGLContext setCurrentContext:nil];
}

} // namespace mbgl
