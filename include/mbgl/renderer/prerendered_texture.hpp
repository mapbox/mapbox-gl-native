#ifndef MBGL_RENDERER_PRERENDERED_TEXTURE
#define MBGL_RENDERER_PRERENDERED_TEXTURE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/platform/gl.hpp>

namespace mbgl {

class StyleBucketRaster;
class Painter;

class PrerenderedTexture : private util::noncopyable {
public:
    PrerenderedTexture(const StyleBucketRaster &properties);
    ~PrerenderedTexture();

    void bindTexture();
    void bindFramebuffer();
    void unbindFramebuffer();

    inline GLuint getTexture() const { return texture; }

    void blur(Painter& painter, uint16_t passes);

public:
    const StyleBucketRaster &properties;

private:
    GLint previous_fbo = 0;
    GLuint fbo = 0;
    GLuint texture = 0;
    GLuint fbo_depth_stencil = 0;
};

}

#endif
