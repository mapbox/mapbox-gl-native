#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/gl/object_store.hpp>

#include <algorithm>
#include <vector>

namespace mbgl {
namespace gl {

class TexturePool::Impl : private util::noncopyable {
public:
    class Pool : private util::noncopyable {
        public:
            Pool() : pool(ObjectStore::get().createTexturePool()), availableIDs(gl::TextureMax) {
                std::copy(pool.get().begin(), pool.get().end(), availableIDs.begin());
            }

            Pool(Pool&& o) : pool(std::move(o.pool)), availableIDs(std::move(o.availableIDs)) {}
            Pool& operator=(Pool&& o) { pool = std::move(o.pool); availableIDs = std::move(o.availableIDs); return *this; }

            gl::UniqueTexturePool pool;
            std::vector<GLuint> availableIDs;
    };

    GLuint acquireTexture() {
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
        pools.emplace_back(Pool {});
        return nextAvailableID(pools.back());
    }

    void releaseTexture(GLuint id) {
        for (auto it = pools.begin(); it != pools.end(); ++it) {
            if (std::find(it->pool.get().begin(), it->pool.get().end(), id) != it->pool.get().end()) {
                it->availableIDs.push_back(id);
                if (GLsizei(it->availableIDs.size()) == gl::TextureMax) {
                    pools.erase(it);
                }
                return;
            }
        }
    }

    std::vector<Pool> pools;
};

void TextureReleaser::operator()(GLuint id) const {
    TexturePool::get().impl->releaseTexture(id);
}

TexturePool::TexturePool() : impl(std::make_unique<Impl>()) {
}

TexturePool::~TexturePool() {
}

PooledTexture TexturePool::acquireTexture() {
    return PooledTexture { impl->acquireTexture() , TextureReleaser {} };
}

void TexturePool::clear() {
    impl->pools.clear();
}

} // namespace gl
} // namespace mbgl
