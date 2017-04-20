#include <mbgl/gl/context.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/render_tile.hpp>

namespace mbgl {
using namespace style;

RasterBucket::RasterBucket(UnassociatedImage&& image_) : image(std::move(image_)) {
}

RasterBucket::RasterBucket(RasterBucket&& other) : image(std::move(other.image)) {
    uploaded = false;
}

void RasterBucket::upload(gl::Context& context) {
    texture = context.createTexture(image);
    if (vertices.vertexSize() > 0) {
        vertexBuffer = context.createVertexBuffer(std::move(vertices));
        indexBuffer = context.createIndexBuffer(std::move(indices));
    }
    uploaded = true;
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const RenderLayer& layer,
                          const RenderTile& tile) {
    painter.renderRaster(parameters, *this, *layer.as<RenderRasterLayer>(), tile.matrix, false);
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const RenderLayer& layer,
                          const mat4& matrix) {
    painter.renderRaster(parameters, *this, *layer.as<RenderRasterLayer>(), matrix, true);
}

bool RasterBucket::hasData() const {
    return true;
}

} // namespace mbgl
