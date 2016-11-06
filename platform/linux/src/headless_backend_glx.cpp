#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <mbgl/platform/log.hpp>

// #include <cassert>

#include <GL/glx.h>

namespace mbgl {

gl::glProc HeadlessBackend::initializeExtension(const char* name) {
    return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
}

void HeadlessBackend::createContext() {
    xDisplay = display->attribute<Display*>();
    fbConfigs = display->attribute<GLXFBConfig*>();

    if (!glContext) {
        // Try to create a legacy context
        glContext = glXCreateNewContext(xDisplay, fbConfigs[0], GLX_RGBA_TYPE, None, True);
        if (glContext) {
            if (!glXIsDirect(xDisplay, glContext)) {
                Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
                glXDestroyContext(xDisplay, glContext);
                glContext = nullptr;
            }
        }
    }

    if (glContext == nullptr) {
        throw std::runtime_error("Error creating GL context object.");
    }

    // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
    // activate the context.
    int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH, 8,
        GLX_PBUFFER_HEIGHT, 8,
        None
    };
    glxPbuffer = glXCreatePbuffer(xDisplay, fbConfigs[0], pbufferAttributes);
}

void HeadlessBackend::destroyContext() {
    if (glxPbuffer) {
        glXDestroyPbuffer(xDisplay, glxPbuffer);
        glxPbuffer = 0;
    }

    glXDestroyContext(xDisplay, glContext);
}

void HeadlessBackend::activateContext() {
    if (!glXMakeContextCurrent(xDisplay, glxPbuffer, glxPbuffer, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessBackend::deactivateContext() {
    if (!glXMakeContextCurrent(xDisplay, 0, 0, nullptr)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
}

} // namespace mbgl
