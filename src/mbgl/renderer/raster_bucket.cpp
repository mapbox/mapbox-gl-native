#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

using namespace mbgl;

RasterBucket::RasterBucket(gl::TexturePool& texturePool)
: raster(texturePool) {
}

void RasterBucket::upload(gl::GLObjectStore& glObjectStore) {
    if (hasData()) {
        raster.upload(glObjectStore);
        uploaded = true;
    }
}

void RasterBucket::render(Painter& painter,
                          const StyleLayer& layer,
                          const UnwrappedTileID& tileID,
                          const mat4& matrix) {
    painter.renderRaster(*this, *layer.as<RasterLayer>(), tileID, matrix);
}

void RasterBucket::setImage(PremultipliedImage image) {
    raster.load(std::move(image));
}

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, gl::GLObjectStore& glObjectStore) {
    raster.bind(true, glObjectStore);
    array.bind(shader, vertices, BUFFER_OFFSET_0, glObjectStore);
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}

bool RasterBucket::needsClipping() const {
    return false;
}
