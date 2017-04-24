#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/render_raster_layer.hpp>
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
                          const RenderLayer& layer,
                          const RenderTile& tile) {
    painter.renderRaster(parameters, *this, *layer.as<RenderRasterLayer>(), tile);
}

bool RasterBucket::hasData() const {
    return true;
}

} // namespace mbgl
