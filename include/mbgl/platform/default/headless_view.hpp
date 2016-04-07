#ifndef MBGL_COMMON_HEADLESS_VIEW
#define MBGL_COMMON_HEADLESS_VIEW

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IOS
#define MBGL_USE_EAGL 1
#else
#define MBGL_USE_CGL 1
#endif
#else
#define MBGL_USE_OSMESA 1
typedef struct osmesa_context* OSMesaContext;
/*
#define GL_GLEXT_PROTOTYPES
#define MBGL_USE_GLX 1
typedef struct _XDisplay Display;
typedef struct __GLXcontextRec* GLXContext;
typedef struct __GLXFBConfigRec* GLXFBConfig;
typedef long unsigned int XID;
typedef XID GLXPbuffer;
*/
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
    ~HeadlessView();

    float getPixelRatio() const override;
    std::array<uint16_t, 2> getSize() const override;
    std::array<uint16_t, 2> getFramebufferSize() const override;

    void activate() override;
    void deactivate() override;
    void notify() override;
    void invalidate() override;
    void beforeRender() override;
    void afterRender() override;
    PremultipliedImage readStillImage() override;

    void resizeFramebuffer();
    void resize(uint16_t width, uint16_t height);

private:
    void loadExtensions();
    bool isActive() const;

    // Implementation specific functions
    static gl::glProc initializeExtension(const char*);
    void createContext();
    void destroyContext();
    void clearBuffers();
    void activateContext();
    void deactivateContext();

private:
    std::shared_ptr<HeadlessDisplay> display;
    const float pixelRatio;
    std::array<uint16_t, 2> dimensions;
    bool needsResize = false;

#if MBGL_USE_CGL
    CGLContextObj glContext = nullptr;
#endif

#if MBGL_USE_EAGL
    void *glContext = nullptr;
#endif

#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
    GLXContext glContext = 0;
    GLXPbuffer glxPbuffer = 0;
#endif

#if MBGL_USE_OSMESA
    OSMesaContext glContext = 0;
    void *pBuffer = nullptr;
#endif

    bool extensionsLoaded = false;

    GLuint fbo = 0;
    GLuint fboDepthStencil = 0;
    GLuint fboColor = 0;

    std::thread::id thread;
};

} // namespace mbgl

#endif
