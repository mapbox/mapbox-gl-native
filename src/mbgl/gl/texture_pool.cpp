#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/gl/object_store.hpp>

#include <vector>

namespace mbgl {
namespace gl {

GLuint TexturePool::getTextureID(gl::ObjectStore& store) {
    for (auto& impl : pools) {
        if (impl.ids.empty()) continue;
        auto it = impl.ids.begin();
        GLuint id = *it;
        impl.ids.erase(it);
        return id;
    }

    // All texture IDs are in use.
    pools.emplace_back(Impl(store));
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
                    pools.erase(it);
                }
                return;
            }
        }
    }
}

} // namespace gl
} // namespace mbgl
