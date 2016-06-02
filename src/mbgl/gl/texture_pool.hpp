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
    void releaseTextureID(GLuint&);

private:
    class Impl : private util::noncopyable {
    public:
        Impl(gl::ObjectStore& store) : pool(store.createTexturePool()), availableIDs(gl::TextureMax) {
            std::copy(pool.get().begin(), pool.get().end(), availableIDs.begin());
        }

        Impl(Impl&& o) : pool(std::move(o.pool)), availableIDs(std::move(o.availableIDs)) {}
        Impl& operator=(Impl&& o) { pool = std::move(o.pool); availableIDs = std::move(o.availableIDs); return *this; }

        gl::UniqueTexturePool pool;
        std::vector<GLuint> availableIDs;
    };

    std::vector<Impl> pools;
};

} // namespace gl
} // namespace mbgl
