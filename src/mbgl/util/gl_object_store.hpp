#ifndef MBGL_MAP_UTIL_GL_OBJECT_STORE
#define MBGL_MAP_UTIL_GL_OBJECT_STORE

#include <mbgl/platform/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <vector>

namespace mbgl {
namespace util {

class GLObjectStore : private util::noncopyable {
public:
    GLObjectStore() = default;

    // Mark OpenGL objects for deletion
    void abandonVAO(GLuint vao);
    void abandonBuffer(GLuint buffer);
    void abandonTexture(GLuint texture);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

private:
    std::vector<GLuint> abandonedVAOs;
    std::vector<GLuint> abandonedBuffers;
    std::vector<GLuint> abandonedTextures;
};

}
}

#endif
