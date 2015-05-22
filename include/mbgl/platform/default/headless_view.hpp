#ifndef MBGL_COMMON_HEADLESS_VIEW
#define MBGL_COMMON_HEADLESS_VIEW

#ifdef __APPLE__
#define MBGL_USE_CGL 1
#else
#define GL_GLEXT_PROTOTYPES
#define MBGL_USE_GLX 1
typedef struct _XDisplay Display;
typedef struct __GLXcontextRec* GLXContext;
typedef struct __GLXFBConfigRec* GLXFBConfig;
typedef long unsigned int XID;
typedef XID GLXPbuffer;
#endif

#include <mbgl/mbgl.hpp>
#include <mbgl/platform/gl.hpp>

#include <memory>
#include <thread>

namespace mbgl {

class HeadlessDisplay;

class HeadlessView : public View {
public:
    HeadlessView(uint16_t width = 256, uint16_t height = 256, float pixelRatio = 1);
    HeadlessView(std::shared_ptr<HeadlessDisplay> display, uint16_t width = 256, uint16_t height = 256, float pixelRatio = 1);
    ~HeadlessView();

    void activate() override;
    void deactivate() override;
    void notify() override;
    void resize(uint16_t width, uint16_t height, float pixelRatio) override;
    void invalidate() override;
    void swap() override;
    std::unique_ptr<StillImage> readStillImage() override;

private:
    void createContext();
    void loadExtensions();
    void clearBuffers();
    bool isActive();

private:
    std::shared_ptr<HeadlessDisplay> display;

    struct Dimensions {
        inline Dimensions(uint16_t width = 0, uint16_t height = 0, float pixelRatio = 0);
        inline uint16_t pixelWidth() const { return width * pixelRatio; }
        inline uint16_t pixelHeight() const { return height * pixelRatio; }

        uint16_t width = 0;
        uint16_t height = 0;
        float pixelRatio = 0;
    };

    Dimensions dimensions;

#if MBGL_USE_CGL
    CGLContextObj glContext = nullptr;
#endif

#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
    GLXContext glContext = 0;
    GLXPbuffer glxPbuffer = 0;
#endif

    bool extensionsLoaded = false;

    GLuint fbo = 0;
    GLuint fboDepthStencil = 0;
    GLuint fboColor = 0;

    std::thread::id thread;
};

}

#endif
