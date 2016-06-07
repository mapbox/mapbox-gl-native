#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/raster_renderable.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/renderer/painter.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(gl::TexturePool& texturePool)
    : renderable(std::make_unique<RasterRenderable>(texturePool)) {
}

RasterBucket::~RasterBucket() = default;

RasterRenderable& RasterBucket::getRenderable() const {
    assert(renderable);
    return *renderable;
}

void RasterBucket::upload(gl::ObjectStore& store) {
    if (hasData()) {
        renderable->raster.upload(store);
    }

    uploaded = true;
}

void RasterBucket::render(Painter& painter,
                          const Layer& layer,
                          const UnwrappedTileID& tileID,
                          const mat4& matrix) {
    painter.renderRaster(*this, *layer.as<RasterLayer>(), tileID, matrix);
}

void RasterBucket::setImage(PremultipliedImage image) {
    renderable->raster.load(std::move(image));
}

bool RasterBucket::hasData() const {
    return renderable && renderable->raster.isLoaded();
}

bool RasterBucket::needsClipping() const {
    return false;
}

} // namespace mbgl
