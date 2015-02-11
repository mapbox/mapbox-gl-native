#ifndef MBGL_UTIL_TEXTUREPOOL
#define MBGL_UTIL_TEXTUREPOOL

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/platform/gl.hpp>

#include <set>
#include <mutex>

namespace mbgl {

class TexturePool : private util::noncopyable {

public:
    GLuint getTextureID();
    void removeTextureID(GLuint texture_id);
    void clearTextureIDs();

private:
    std::set<GLuint> texture_ids;
};

}

#endif
