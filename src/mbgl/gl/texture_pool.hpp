#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>

#include <memory>

namespace mbgl {
namespace gl {

class TexturePool : private util::noncopyable {
public:
    TexturePool();
    ~TexturePool();

    GLuint getTextureID(gl::ObjectStore&);
    void releaseTextureID(GLuint&);

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace gl
} // namespace mbgl
