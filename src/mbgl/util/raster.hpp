#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

#include <atomic>
#include <array>
#include <vector>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class Raster {
public:
    enum class MipMap : bool { No = false, Yes = true };
    enum class Scaling : bool { Nearest = false, Linear = true };

    // load image data
    void load(PremultipliedImage, uint32_t mipmapLevel = 0);

    // bind current texture
    void bind(gl::Context&,
              uint32_t unit,
              Scaling = Scaling::Nearest,
              MipMap = MipMap::No);

    // uploads the texture if it hasn't been uploaded yet.
    void upload(gl::Context&, uint32_t unit);

    // loaded status
    bool isLoaded() const;

    // returns the OpenGL texture ID
    gl::TextureID getID() const;

    // size of uploaded image.
    std::array<uint16_t, 2> getSize() const;

private:
    void updateFilter();

private:
    // raw pixels have been loaded
    std::atomic<bool> loaded { false };
    std::array<uint16_t, 2> size = {{ 0, 0 }};

    // filters
    Scaling filter = Scaling::Nearest;
    MipMap mipmap = MipMap::No;

    // the raw pixels
    std::vector<PremultipliedImage> images;

    // GL buffer object handle.
    mbgl::optional<gl::UniqueTexture> texture;
};

} // namespace mbgl
