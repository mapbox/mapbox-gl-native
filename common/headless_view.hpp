#ifndef MBGL_COMMON_HEADLESS_CGL
#define MBGL_COMMON_HEADLESS_CGL

#ifdef __APPLE__
#define MBGL_USE_CGL 1
#else
#include <GL/glx.h>
#define MBGL_USE_GLX 1
#endif

#include <mbgl/map/view.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/time.hpp>

namespace mbgl {

class HeadlessView : public View {
public:
    HeadlessView();
    ~HeadlessView();

    void resize(uint16_t width, uint16_t height, float pixelRatio);

    void notify_map_change(MapChange change, timestamp delay = 0);
    void make_active();
    void swap();
    unsigned int root_fbo();

private:
    void clear_buffers();


private:
#if MBGL_USE_CGL
    CGLContextObj gl_context;
    GLuint fbo = 0;
    GLuint fbo_depth_stencil = 0;
    GLuint fbo_color = 0;
#endif

#if MBGL_USE_GLX
    GLXContext gl_context = nullptr;
    XVisualInfo *x_info = nullptr;
    Display *x_display = nullptr;
    Pixmap x_pixmap = 0;
    GLXPixmap glx_pixmap = 0;
#endif
};

}

#endif
