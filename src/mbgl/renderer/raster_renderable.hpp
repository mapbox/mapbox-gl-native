#pragma once

#include <mbgl/util/raster.hpp>

namespace mbgl {

class RasterRenderable {
public:
    inline RasterRenderable(gl::TexturePool& texturePool) : raster(texturePool) {
    }

    Raster raster;
};

} // namespace mbgl
