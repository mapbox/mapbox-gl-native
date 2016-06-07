#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/util/raster.hpp>

namespace mbgl {

class RasterRenderable : public Renderable {
public:
    inline RasterRenderable(gl::TexturePool& texturePool) : raster(texturePool) {
    }

    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            raster.upload(store);
            uploaded = true;
        }
    }

    Raster raster;
};

} // namespace mbgl
