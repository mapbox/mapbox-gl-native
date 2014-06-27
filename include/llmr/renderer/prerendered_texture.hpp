#ifndef LLMR_RENDERER_PRERENDERED_TEXTURE
#define LLMR_RENDERER_PRERENDERED_TEXTURE

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/style/rasterize_properties.hpp>

namespace llmr {

class Painter;

class PrerenderedTexture : private util::noncopyable {
public:
    PrerenderedTexture(const PrerenderProperties &properties);
    ~PrerenderedTexture();

    void bindTexture();
    void bindFramebuffer();
    void unbindFramebuffer();

    inline GLuint getTexture() const { return texture; }

    void blur(Painter& painter, uint16_t passes);

public:
    const PrerenderProperties properties;

private:
    GLint previous_fbo = 0;
    GLuint fbo = 0;
    GLuint texture = 0;
};

}

#endif
