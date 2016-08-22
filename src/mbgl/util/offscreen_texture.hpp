#pragma once

#include <mbgl/util/raster.hpp>

namespace mbgl {

namespace gl {
class Config;
} // namespace gl

class OffscreenTexture {
public:
    void bind(gl::ObjectStore&, gl::Config&, std::array<uint16_t, 2> size);

    Raster& getTexture();
    std::array<uint16_t, 2> getSize() const;

private:
    mbgl::optional<gl::UniqueFBO> fbo;
    Raster raster;
};

} // namespace mbgl
