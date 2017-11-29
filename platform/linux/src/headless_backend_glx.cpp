#include <mbgl/gl/headless_backend.hpp>

#include <mbgl/util/logging.hpp>

#include <cassert>

#include <GL/glx.h>

namespace mbgl {

// This class provides a singleton that contains information about the configuration used for
// instantiating new headless rendering contexts.
class GLXDisplayConfig {
private:
    // Key for singleton construction.
    struct Key { explicit Key() = default; };

public:
    GLXDisplayConfig(Key) {
        if (!XInitThreads()) {
            throw std::runtime_error("Failed to XInitThreads.");
        }

        xDisplay = XOpenDisplay(nullptr);
        if (xDisplay == nullptr) {
            throw std::runtime_error("Failed to open X display.");
        }

        const auto* extensions = reinterpret_cast<const char*>(
            glXQueryServerString(xDisplay, DefaultScreen(xDisplay), GLX_EXTENSIONS));
        if (!extensions) {
            throw std::runtime_error("Cannot read GLX extensions.");
        }
        if (!strstr(extensions, "GLX_SGIX_fbconfig")) {
            throw std::runtime_error("Extension GLX_SGIX_fbconfig was not found.");
        }
        if (!strstr(extensions, "GLX_SGIX_pbuffer")) {
            throw std::runtime_error("Cannot find glXCreateContextAttribsARB.");
        }

        // We're creating a dummy pbuffer anyway that we're not using.
        static int pixelFormat[] = { GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, None };

        int configs = 0;
        fbConfigs = glXChooseFBConfig(xDisplay, DefaultScreen(xDisplay), pixelFormat, &configs);
        if (fbConfigs == nullptr) {
            throw std::runtime_error("Failed to glXChooseFBConfig.");
        }
        if (configs <= 0) {
            throw std::runtime_error("No Framebuffer configurations.");
        }
    }

    ~GLXDisplayConfig() {
        XFree(fbConfigs);
        XCloseDisplay(xDisplay);
    }

    static std::shared_ptr<const GLXDisplayConfig> create() {
        static std::weak_ptr<const GLXDisplayConfig> instance;
        auto shared = instance.lock();
        if (!shared) {
            instance = shared = std::make_shared<GLXDisplayConfig>(Key{});
        }
        return shared;
    }

public:
    Display* xDisplay = nullptr;
    GLXFBConfig* fbConfigs = nullptr;
};

class GLXBackendImpl : public HeadlessBackend::Impl {
public:
    GLXBackendImpl() {
        // Try to create a legacy context.
        glContext = glXCreateNewContext(glxDisplay->xDisplay, glxDisplay->fbConfigs[0],
                                        GLX_RGBA_TYPE, None, True);
        if (glContext && !glXIsDirect(glxDisplay->xDisplay, glContext)) {
            Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
            glXDestroyContext(glxDisplay->xDisplay, glContext);
            glContext = nullptr;
        }
        if (glContext == nullptr) {
            throw std::runtime_error("Error creating GL context object.");
        }

        // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
        // activate the context.
        int pbufferAttributes[] = { GLX_PBUFFER_WIDTH, 8, GLX_PBUFFER_HEIGHT, 8, None };
        glxPbuffer =
            glXCreatePbuffer(glxDisplay->xDisplay, glxDisplay->fbConfigs[0], pbufferAttributes);
    }

    ~GLXBackendImpl() final {
        if (glxPbuffer) {
            glXDestroyPbuffer(glxDisplay->xDisplay, glxPbuffer);
        }
        glXDestroyContext(glxDisplay->xDisplay, glContext);
    }

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
        return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
    }

    void activateContext() final {
        if (!glXMakeContextCurrent(glxDisplay->xDisplay, glxPbuffer, glxPbuffer, glContext)) {
            throw std::runtime_error("Switching OpenGL context failed.\n");
        }
    }

    void deactivateContext() final {
        if (!glXMakeContextCurrent(glxDisplay->xDisplay, 0, 0, nullptr)) {
            throw std::runtime_error("Removing OpenGL context failed.\n");
        }
    }

private:
    const std::shared_ptr<const GLXDisplayConfig> glxDisplay = GLXDisplayConfig::create();

    GLXContext glContext = nullptr;
    GLXPbuffer glxPbuffer = 0;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<GLXBackendImpl>();
}

} // namespace mbgl
