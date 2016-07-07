#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

namespace mbgl {

using namespace style;

void RasterBucket::upload(gl::ObjectStore& store, gl::Config& config) {
    if (hasData()) {
        raster.upload(store, config, 0);
        uploaded = true;
    }
}

void RasterBucket::render(Painter& painter,
                          const Layer& layer,
                          const UnwrappedTileID& tileID,
                          const mat4& matrix) {
    painter.renderRaster(*this, *layer.as<RasterLayer>(), tileID, matrix);
}

void RasterBucket::setImage(PremultipliedImage image) {
    raster.load(std::move(image));
}

void RasterBucket::drawRaster(RasterShader& shader,
                              StaticRasterVertexBuffer& vertices,
                              VertexArrayObject& array,
                              gl::Config& config,
                              gl::ObjectStore& store) {
    raster.bind(store, config, 0, Raster::Scaling::Linear);
    raster.bind(store, config, 1, Raster::Scaling::Linear);
    array.bind(shader, vertices, BUFFER_OFFSET_0, store);
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}

bool RasterBucket::needsClipping() const {
    return false;
}

} // namespace mbgl
