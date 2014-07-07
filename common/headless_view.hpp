#ifndef LLMR_COMMON_HEADLESS_CGL
#define LLMR_COMMON_HEADLESS_CGL

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
    CGLContextObj gl_context;
    GLuint fbo = 0;
    GLuint fbo_depth_stencil = 0;
    GLuint fbo_color = 0;
};

}

#endif
