#include <mbgl/gl/headless_backend.hpp>

#include <mbgl/util/logging.hpp>

#include <EGL/egl.h>

#include <cassert>

namespace mbgl {

// This class provides a singleton that contains information about the configuration used for
// instantiating new headless rendering contexts.
class EGLDisplayConfig {
private:
    // Key for singleton construction.
    struct Key { explicit Key() = default; };

public:
    EGLDisplayConfig(Key) {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to obtain a valid EGL display.\n");
        }

        EGLint major, minor, numConfigs;
        if (!eglInitialize(display, &major, &minor)) {
            throw std::runtime_error("eglInitialize() failed.\n");
        }

        if (!eglBindAPI(EGL_OPENGL_ES_API)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglBindAPI(EGL_OPENGL_ES_API) returned error %d",
                             eglGetError());
            throw std::runtime_error("eglBindAPI() failed");
        }

        const EGLint attribs[] = {
#if MBGL_USE_GLES2
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_NONE
        };

        // Note: we're choosing an arbitrary pixel format, since we're not using the default surface
        // anyway; all rendering will be directed to framebuffers which have their own configuration.
        if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs) || numConfigs != 1) {
            throw std::runtime_error("Failed to choose ARGB config.\n");
        }
    }

    ~EGLDisplayConfig() {
        eglTerminate(display);
    }

    static std::shared_ptr<const EGLDisplayConfig> create() {
        static std::weak_ptr<const EGLDisplayConfig> instance;
        auto shared = instance.lock();
        if (!shared) {
            instance = shared = std::make_shared<EGLDisplayConfig>(Key{});
        }
        return shared;
    }

public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig config = 0;
};

class EGLBackendImpl : public HeadlessBackend::Impl {
public:
    EGLBackendImpl() {
        // EGL initializes the context client version to 1 by default. We want to
        // use OpenGL ES 2.0 which has the ability to create shader and program
        // objects and also to write vertex and fragment shaders in the OpenGL ES
        // Shading Language.
        const EGLint attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        eglContext = eglCreateContext(eglDisplay->display, eglDisplay->config, EGL_NO_CONTEXT, attribs);
        if (eglContext == EGL_NO_CONTEXT) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateContext() returned error 0x%04x",
                             eglGetError());
            throw std::runtime_error("Error creating the EGL context object.\n");
        }

        // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
        // activate the context.
        // Note that to be able to create pbuffer surfaces, we need to choose a config that
        // includes EGL_SURFACE_TYPE, EGL_PBUFFER_BIT in HeadlessDisplay.
        const EGLint surfAttribs[] = {
            EGL_WIDTH, 8,
            EGL_HEIGHT, 8,
            EGL_LARGEST_PBUFFER, EGL_TRUE,
            EGL_NONE
        };

        eglSurface = eglCreatePbufferSurface(eglDisplay->display, eglDisplay->config, surfAttribs);
        if (eglSurface == EGL_NO_SURFACE) {
            throw std::runtime_error("Could not create surface: " + std::to_string(eglGetError()));
        }
    }

    ~EGLBackendImpl() final {
        if (eglSurface != EGL_NO_SURFACE) {
            if (!eglDestroySurface(eglDisplay->display, eglSurface)) {
                Log::Error(Event::OpenGL, "Failed to destroy EGL surface.");
            }
            eglSurface = EGL_NO_SURFACE;
        }
        if (!eglDestroyContext(eglDisplay->display, eglContext)) {
            Log::Error(Event::OpenGL, "Failed to destroy EGL context.");
        }
    }

    gl::ProcAddress getExtensionFunctionPointer(const char* name) final {
        return eglGetProcAddress(name);
    }

    void activateContext() final {
        if (!eglMakeCurrent(eglDisplay->display, eglSurface, eglSurface, eglContext)) {
            throw std::runtime_error("Switching OpenGL context failed.\n");
        }
    }

    void deactivateContext() final {
        if (!eglMakeCurrent(eglDisplay->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            throw std::runtime_error("Removing OpenGL context failed.\n");
        }
    }

private:
    const std::shared_ptr<const EGLDisplayConfig> eglDisplay = EGLDisplayConfig::create();
    EGLContext eglContext = EGL_NO_CONTEXT;
    EGLSurface eglSurface = EGL_NO_SURFACE;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<EGLBackendImpl>();
}

} // namespace mbgl
