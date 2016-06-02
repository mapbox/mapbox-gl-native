#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/gl/object_store.hpp>

#include <algorithm>
#include <vector>

namespace mbgl {
namespace gl {

GLuint TexturePool::getTextureID(gl::ObjectStore& store) {
    auto nextAvailableID = [](auto& pool_) {
        auto it = pool_.availableIDs.begin();
        GLuint id = *it;
        pool_.availableIDs.erase(it);
        return id;
    };

    for (auto& pool : pools) {
        if (pool.availableIDs.empty()) continue;
        return nextAvailableID(pool);
    }

    // All texture IDs are in use.
    pools.emplace_back(Impl { store });
    return nextAvailableID(pools.back());
}

void TexturePool::releaseTextureID(GLuint& id) {
    for (auto it = pools.begin(); it != pools.end(); ++it) {
        if (std::find(it->pool.get().begin(), it->pool.get().end(), id) != it->pool.get().end()) {
            it->availableIDs.push_back(id);
            id = 0;
            if (GLsizei(it->availableIDs.size()) == gl::TextureMax) {
                pools.erase(it);
            }
            return;
        }
    }
}

} // namespace gl
} // namespace mbgl
