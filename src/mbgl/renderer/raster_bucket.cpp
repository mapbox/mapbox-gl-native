#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

using namespace mbgl;

RasterBucket::RasterBucket(gl::TexturePool& texturePool)
: raster(texturePool) {
}

void RasterBucket::upload() {
    if (hasData()) {
        raster.upload();
        uploaded = true;
    }
}

void RasterBucket::render(Painter& painter,
                          const StyleLayer& layer,
                          const TileID& id,
                          const mat4& matrix) {
    painter.renderRaster(*this, *layer.as<RasterLayer>(), id, matrix);
}

void RasterBucket::setImage(PremultipliedImage image) {
    raster.load(std::move(image));
}

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array) {
    raster.bind(true);
    shader.u_image = 0;
    array.bind(shader, vertices, BUFFER_OFFSET_0);
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
