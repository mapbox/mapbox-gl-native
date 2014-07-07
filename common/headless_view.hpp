#ifndef LLMR_COMMON_HEADLESS_CGL
#define LLMR_COMMON_HEADLESS_CGL

#ifdef __APPLE__
#define LLMR_USE_CGL 1
#else
#include <GL/glx.h>
#define LLMR_USE_GLX 1
#endif

#include <llmr/map/view.hpp>
#include <llmr/platform/gl.hpp>

namespace llmr {

class HeadlessView : public View {
public:
    HeadlessView();
    ~HeadlessView();

    void resize(int width, int height);

    void make_active();
    void swap();
    unsigned int root_fbo();

private:
    void clear_buffers();


private:
#ifdef LLMR_USE_CGL
    CGLContextObj gl_context;
    GLuint fbo = 0;
    GLuint fbo_depth_stencil = 0;
    GLuint fbo_color = 0;
#endif

#ifdef LLMR_USE_GLX
    GLXContext gl_context = nullptr;
    XVisualInfo *x_info = nullptr;
    Display *x_display = nullptr;
    Pixmap x_pixmap = 0;
    GLXPixmap glx_pixmap = 0;
#endif
};

}

#endif
