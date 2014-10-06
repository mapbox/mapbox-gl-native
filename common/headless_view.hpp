#ifndef MBGL_COMMON_HEADLESS_VIEW
#define MBGL_COMMON_HEADLESS_VIEW

#ifdef __APPLE__
#define MBGL_USE_CGL 1
#else
#include "glx.h"
#define MBGL_USE_GLX 1
#endif

#include <mbgl/map/view.hpp>
#include <mbgl/platform/gl.hpp>

namespace mbgl {

class HeadlessDisplay;

class HeadlessView : public View {
public:
    HeadlessView();
    HeadlessView(HeadlessDisplay *display);
    ~HeadlessView();

    void createContext();

    void resize(uint16_t width, uint16_t height, float pixelRatio);

    void notify_map_change(MapChange change, timestamp delay = 0);
    void make_active();
    void make_inactive();
    void swap();
    unsigned int root_fbo();

private:
    void clear_buffers();


private:
    HeadlessDisplay *display_;

#if MBGL_USE_CGL
    CGLContextObj gl_context;
    GLuint fbo = 0;
    GLuint fbo_depth_stencil = 0;
    GLuint fbo_color = 0;
#endif

#if MBGL_USE_GLX
    Display *x_display = nullptr;
    XVisualInfo *x_info = nullptr;
    GLXContext gl_context = nullptr;
    Pixmap x_pixmap = 0;
    GLXPixmap glx_pixmap = 0;
#endif
};

}

#endif
