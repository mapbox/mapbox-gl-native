#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/headless_display.hpp>

#include <mbgl/util/logging.hpp>

#include <cassert>

#include <GL/glx.h>

namespace mbgl {

struct GLXImpl : public HeadlessBackend::Impl {
    GLXImpl(GLXContext glContext_, GLXPbuffer glxPbuffer_, Display* xDisplay_, GLXFBConfig* fbConfigs_)
            : glContext(glContext_),
              glxPbuffer(glxPbuffer_),
              xDisplay(xDisplay_),
              fbConfigs(fbConfigs_) {
    }

    ~GLXImpl() {
        if (glxPbuffer) {
            glXDestroyPbuffer(xDisplay, glxPbuffer);
        }
        glXDestroyContext(xDisplay, glContext);
    }

    void activateContext() final {
        if (!glXMakeContextCurrent(xDisplay, glxPbuffer, glxPbuffer, glContext)) {
            throw std::runtime_error("Switching OpenGL context failed.\n");
        }
    }

    void deactivateContext() final {
        if (!glXMakeContextCurrent(xDisplay, 0, 0, nullptr)) {
            throw std::runtime_error("Removing OpenGL context failed.\n");
        }
    }

    GLXContext glContext = nullptr;
    GLXPbuffer glxPbuffer = 0;

    // Needed for ImplDeleter.
    Display* xDisplay = nullptr;
    GLXFBConfig* fbConfigs = nullptr;
};

gl::ProcAddress HeadlessBackend::initializeExtension(const char* name) {
    return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
}

bool HeadlessBackend::hasDisplay() {
    if (!display) {
        display.reset(new HeadlessDisplay);
    }
    return bool(display);
};

void HeadlessBackend::createContext() {
    assert(!hasContext());

    Display* xDisplay = display->attribute<Display*>();
    GLXFBConfig* fbConfigs = display->attribute<GLXFBConfig*>();

    // Try to create a legacy context.
    GLXContext glContext = glXCreateNewContext(xDisplay, fbConfigs[0], GLX_RGBA_TYPE, None, True);
    if (glContext && !glXIsDirect(xDisplay, glContext)) {
        Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
        glXDestroyContext(xDisplay, glContext);
        glContext = nullptr;
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
    GLXPbuffer glxPbuffer = glXCreatePbuffer(xDisplay, fbConfigs[0], pbufferAttributes);

    impl.reset(new GLXImpl(glContext, glxPbuffer, xDisplay, fbConfigs));
}

} // namespace mbgl
