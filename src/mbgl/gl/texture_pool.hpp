#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

#include <algorithm>
#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

class TexturePool : private util::noncopyable {
public:
    GLuint getTextureID(gl::ObjectStore&);
    void releaseTextureID(GLuint);

private:
    class Impl : private util::noncopyable {
    public:
        Impl(gl::ObjectStore& store) : ids(gl::TexturePoolHolder::TextureMax) {
            pool.create(store);
            std::copy(pool.getIDs().begin(), pool.getIDs().end(), ids.begin());
        }

        Impl(Impl&& o) : pool(std::move(o.pool)), ids(std::move(o.ids)) {}
        Impl& operator=(Impl&& o) { pool = std::move(o.pool); ids = std::move(o.ids); return *this; }

        gl::TexturePoolHolder pool;
        std::vector<GLuint> ids;
    };

    std::vector<Impl> pools;
};

} // namespace gl
} // namespace mbgl
