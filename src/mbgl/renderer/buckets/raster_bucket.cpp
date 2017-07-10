#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/renderer/render_tile.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(UnassociatedImage&& image_) : image(std::move(image_)) {
}

void RasterBucket::upload(gl::Context& context) {
    if (!texture) {
        texture = context.createTexture(image);
    }
    if (!primitives.empty()) {
        drawable.emplace(context, std::move(primitives));
    }
    uploaded = true;
}

void RasterBucket::clear() {
    primitives = {};
    drawable = std::experimental::nullopt;
    uploaded = false;
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const RenderLayer& layer,
                          const RenderTile& tile) {
    painter.renderRaster(parameters, *this, *layer.as<RenderRasterLayer>(), tile.matrix,
                         painter.rasterDrawable);
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const RenderLayer& layer,
                          const mat4& matrix) {
    if (drawable) {
        painter.renderRaster(parameters, *this, *layer.as<RenderRasterLayer>(), matrix, *drawable);
    }
}

bool RasterBucket::hasData() const {
    return true;
}

} // namespace mbgl
