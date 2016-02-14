#ifndef MBGL_UTIL_TEXTUREPOOL
#define MBGL_UTIL_TEXTUREPOOL

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/gl.hpp>

#include <set>

namespace mbgl {
namespace gl {

class TexturePool : private util::noncopyable {

public:
    GLuint getTextureID();
    void removeTextureID(GLuint texture_id);

private:
    std::set<GLuint> texture_ids;
};

} // namespace gl
} // namespace mbgl

#endif
