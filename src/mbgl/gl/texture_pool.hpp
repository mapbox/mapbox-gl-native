#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

#include <unique_resource.hpp>

#include <memory>

namespace mbgl {
namespace gl {

struct TextureReleaser {
    void operator()(GLuint) const;
};

using PooledTexture = std_experimental::unique_resource<GLuint, TextureReleaser>;

class TexturePool : private util::noncopyable {
public:
    static TexturePool& get() {
        static TexturePool pool;
        return pool;
    }

    ~TexturePool();

    PooledTexture acquireTexture();
    void clear();

private:
    TexturePool();

    friend TextureReleaser;

    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace gl
} // namespace mbgl
