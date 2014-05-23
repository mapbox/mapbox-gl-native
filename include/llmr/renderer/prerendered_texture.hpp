#ifndef LLMR_RENDERER_PRERENDERED_TEXTURE
#define LLMR_RENDERER_PRERENDERED_TEXTURE

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/gl.hpp>

namespace llmr {

class PrerenderedTexture : private util::noncopyable {
public:
    ~PrerenderedTexture();

    void bindTexture();
    void bindFramebuffer();
    void unbindFramebuffer();

    inline GLuint getTexture() const { return texture; }

private:
    GLint previous_fbo = 0;
    GLuint fbo = 0;
    GLuint texture = 0;
    bool mipmapped = false;
};

}

#endif
