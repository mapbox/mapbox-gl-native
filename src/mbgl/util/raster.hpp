#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/texture_pool.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <mutex>

namespace mbgl {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    Raster(gl::TexturePool&);

    // load image data
    void load(PremultipliedImage);

    // bind current texture
    void bind(bool linear, gl::ObjectStore&);

    // uploads the texture if it hasn't been uploaded yet.
    void upload(gl::ObjectStore&);

    // loaded status
    bool isLoaded() const;

public:
    // loaded image dimensions
    GLsizei width = 0;
    GLsizei height = 0;

    // GL buffer object handle.
    mbgl::optional<gl::SharedTexture> texture;

    // texture opacity
    double opacity = 0;

private:
    mutable std::mutex mtx;

    // raw pixels have been loaded
    bool loaded = false;

    // shared texture pool
    gl::TexturePool& texturePool;

    // min/mag filter
    GLint filter = 0;

    // the raw pixels
    PremultipliedImage img;
};

} // namespace mbgl
