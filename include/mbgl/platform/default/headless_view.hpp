#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#define MBGL_USE_EAGL 1
#else
#define MBGL_USE_CGL 1
#endif
#else
#define MBGL_USE_EGL 1
struct gbm_surface {};
struct gbm_device {};
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLConfig;
typedef void* EGLSurface;
#endif

#include <mbgl/mbgl.hpp>
#include <mbgl/gl/gl.hpp>

#include <memory>
#include <thread>

namespace mbgl {

class HeadlessDisplay;

class HeadlessView : public View {
public:
    HeadlessView(float pixelRatio, uint16_t width = 256, uint16_t height = 256);
    HeadlessView(std::shared_ptr<HeadlessDisplay> display, float pixelRatio, uint16_t width = 256, uint16_t height = 256);
    ~HeadlessView() override;

    float getPixelRatio() const override;
    std::array<uint16_t, 2> getSize() const override;
    std::array<uint16_t, 2> getFramebufferSize() const override;

    void invalidate() override;
    void activate() override;
    void deactivate() override;

    PremultipliedImage readStillImage() override;

    void resize(uint16_t width, uint16_t height);

private:
    // Implementation specific functions
    static gl::glProc initializeExtension(const char*);
    void createContext();
    void destroyContext();
    void clearBuffers();
    void resizeFramebuffer();
    void activateContext();
    void deactivateContext();

    std::shared_ptr<HeadlessDisplay> display;
    const float pixelRatio;
    std::array<uint16_t, 2> dimensions;

    bool needsResize = false;
    bool extensionsLoaded = false;
    bool active = false;

#if MBGL_USE_CGL
    CGLContextObj glContext = nullptr;
#endif

#if MBGL_USE_EAGL
    void *glContext = nullptr;
#endif

#if MBGL_USE_EGL
    EGLDisplay dpy;
    EGLContext glContext = nullptr;
    EGLConfig config;
    EGLSurface surface;
    struct gbm_surface *gs = nullptr;
#endif

    GLuint fbo = 0;
    GLuint fboDepthStencil = 0;
    GLuint fboColor = 0;
};

} // namespace mbgl
