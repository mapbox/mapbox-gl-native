#ifndef LLMR_COMMON_HEADLESS_CGL
#define LLMR_COMMON_HEADLESS_CGL

#ifdef __APPLE__
#define LLMR_USE_CGL 1
#define LLMR_CONTEXT_OBJ CGLContextObj
#else
#include <GL/glx.h>
#define LLMR_USE_GLX 1
#define LLMR_CONTEXT_OBJ GLXContext
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
    LLMR_CONTEXT_OBJ gl_context;

#ifdef LLMR_USE_CGL
    GLuint fbo = 0;
    GLuint fbo_depth_stencil = 0;
    GLuint fbo_color = 0;
#endif

#ifdef LLMR_USE_GLX
    XVisualInfo *x_info = nullptr;
    Display *x_display = nullptr;
    Pixmap x_pixmap;
    GLXPixmap glx_pixmap;
#endif
};

}

#endif
