#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <CoreFoundation/CoreFoundation.h>

#include <string>
#include <stdexcept>

namespace mbgl {

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<gl::glProc>(symbol);
}

void HeadlessBackend::createContext() {
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
}

void HeadlessBackend::destroyContext() {
    CGLDestroyContext(glContext);
}

void HeadlessBackend::activateContext() {
    CGLError error = CGLSetCurrentContext(glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Switching OpenGL context failed:") +
                                 CGLErrorString(error) + "\n");
    }
}

void HeadlessBackend::deactivateContext() {
    CGLError error = CGLSetCurrentContext(nullptr);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Removing OpenGL context failed:") +
                                 CGLErrorString(error) + "\n");
    }
}

} // namespace mbgl
