#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

#include <unique_resource.hpp>

#include <memory>

namespace mbgl {
namespace gl {

class TexturePool;

struct TextureReleaser {
    TexturePool* pool;
    void operator()(GLuint) const;
};

using PooledTexture = std_experimental::unique_resource<GLuint, TextureReleaser>;

class TexturePool : private util::noncopyable {
public:
    TexturePool();
    ~TexturePool();

    PooledTexture acquireTexture(gl::ObjectStore&);

private:
    friend TextureReleaser;

    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace gl
} // namespace mbgl
