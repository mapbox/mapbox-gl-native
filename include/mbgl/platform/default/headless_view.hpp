#ifndef MBGL_COMMON_HEADLESS_VIEW
#define MBGL_COMMON_HEADLESS_VIEW

#ifdef __APPLE__
#define MBGL_USE_CGL 1
#else
#define GL_GLEXT_PROTOTYPES
#include <mbgl/platform/default/glx.h>
#define MBGL_USE_GLX 1
#endif

#include <mbgl/map/view.hpp>
#include <mbgl/platform/gl.hpp>

#include <memory>

namespace mbgl {

class HeadlessDisplay;

class HeadlessView : public View {
public:
    HeadlessView();
    HeadlessView(std::shared_ptr<HeadlessDisplay> display);
    ~HeadlessView();

    void createContext();
    void loadExtensions();

    void resize(uint16_t width, uint16_t height, float pixelRatio);
    std::unique_ptr<uint32_t[]> readPixels();

    void notify();
    void notifyMapChange(MapChange change, std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero());
    void activate();
    void deactivate();
    void swap();

private:
    void clearBuffers();

private:
    std::shared_ptr<HeadlessDisplay> display_;
    uint16_t width_;
    uint16_t height_;
    float pixelRatio_;

#if MBGL_USE_CGL
    CGLContextObj glContext;
#endif

#if MBGL_USE_GLX
    Display *xDisplay = nullptr;
    GLXFBConfig *fbConfigs = nullptr;
    GLXContext glContext = 0;
    GLXPbuffer glxPbuffer = 0;
#endif

    GLuint fbo = 0;
    GLuint fboDepthStencil = 0;
    GLuint fboColor = 0;
};

}

#endif
