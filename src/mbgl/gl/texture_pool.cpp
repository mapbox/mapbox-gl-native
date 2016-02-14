#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>

#include <vector>

namespace mbgl {
namespace gl {

GLuint TexturePool::getTextureID() {
    if (pools.empty()) pools.emplace_back();

    for (auto& impl : pools) {
        if (impl.ids.empty()) continue;
        auto it = impl.ids.begin();
        GLuint id = *it;
        impl.ids.erase(it);
        return id;
    }

    // All texture IDs are in use.
    pools.emplace_back();
    auto it = pools.back().ids.begin();
    GLuint id = *it;
    pools.back().ids.erase(it);
    return id;
}

void TexturePool::releaseTextureID(GLuint id) {
    for (auto it = pools.begin(); it != pools.end(); ++it) {
        for (GLsizei i = 0; i < gl::TexturePoolHolder::TextureMax; ++i) {
            if (it->pool[i] == id) {
                it->ids.push_back(id);
                if (GLsizei(it->ids.size()) == gl::TexturePoolHolder::TextureMax) {
                    util::ThreadContext::getGLObjectStore()->abandon(std::move(it->pool));
                    pools.erase(it);
                }
                return;
            }
        }
    }
}

} // namespace gl
} // namespace mbgl
