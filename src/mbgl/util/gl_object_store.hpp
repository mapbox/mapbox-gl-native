#ifndef MBGL_MAP_UTIL_GL_OBJECT_STORE
#define MBGL_MAP_UTIL_GL_OBJECT_STORE

#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <vector>

namespace mbgl {
namespace util {

class GLObjectStore : private util::noncopyable {
public:
    GLObjectStore() = default;

    // Mark OpenGL objects for deletion
    void abandonVAO(uint32_t vao);
    void abandonBuffer(uint32_t buffer);
    void abandonTexture(uint32_t texture);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

private:
    std::vector<uint32_t> abandonedVAOs;
    std::vector<uint32_t> abandonedBuffers;
    std::vector<uint32_t> abandonedTextures;
};

}
}

#endif
