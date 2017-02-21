#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(UnassociatedImage&& image_) : image(std::move(image_)) {
}

void RasterBucket::upload(gl::Context& context) {
    texture = context.createTexture(std::move(image));
    uploaded = true;
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const Layer& layer,
                          const RenderTile& tile,
                          const Style&) {
    painter.renderRaster(parameters, *this, *layer.as<RasterLayer>(), tile);
}

bool RasterBucket::hasData() const {
    return true;
}

} // namespace mbgl
