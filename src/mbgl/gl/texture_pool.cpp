#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/gl/object_store.hpp>

#include <algorithm>
#include <vector>

namespace mbgl {
namespace gl {

class TexturePool::Impl : private util::noncopyable {
public:
    class Group : private util::noncopyable {
        public:
            Group(gl::ObjectStore& store) : pool(store.createTexturePool()), availableIDs(gl::TextureMax) {
                std::copy(pool.get().begin(), pool.get().end(), availableIDs.begin());
            }

            Group(Group&& o) : pool(std::move(o.pool)), availableIDs(std::move(o.availableIDs)) {}
            Group& operator=(Group&& o) { pool = std::move(o.pool); availableIDs = std::move(o.availableIDs); return *this; }

            gl::UniqueTexturePool pool;
            std::vector<GLuint> availableIDs;
    };

    GLuint getTextureID(gl::ObjectStore& store) {
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
        pools.emplace_back(Group { store });
        return nextAvailableID(pools.back());
    }

    void releaseTextureID(GLuint& id) {
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

private:
    std::vector<Group> pools;
};

TexturePool::TexturePool() : impl(std::make_unique<Impl>()) {
}

TexturePool::~TexturePool() {
}

GLuint TexturePool::getTextureID(gl::ObjectStore& store) {
    return impl->getTextureID(store);
}

void TexturePool::releaseTextureID(GLuint& id) {
    impl->releaseTextureID(id);
}

} // namespace gl
} // namespace mbgl
