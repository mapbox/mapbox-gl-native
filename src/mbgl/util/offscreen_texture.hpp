#pragma once

#include <mbgl/util/raster.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class OffscreenTexture {
public:
    void bind(gl::Context&, std::array<uint16_t, 2> size);

    Raster& getTexture();
    std::array<uint16_t, 2> getSize() const;

private:
    mbgl::optional<gl::UniqueFramebuffer> framebuffer;
    Raster raster;
};

} // namespace mbgl
