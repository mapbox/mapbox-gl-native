#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

using namespace mbgl;

RasterBucket::RasterBucket(TexturePool& texturePool, const StyleLayoutRaster& layout_)
: layout(layout_),
  raster(texturePool) {
}

void RasterBucket::upload() {
    if (hasData()) {
        raster.upload();
        uploaded = true;
    }
}

void RasterBucket::render(Painter& painter,
                          const StyleLayer& layer_desc,
                          const TileID& id,
                          const mat4& matrix) {
    painter.renderRaster(*this, layer_desc, id, matrix);
}

bool RasterBucket::setImage(std::unique_ptr<util::Image> image) {
    return raster.load(std::move(image));
}

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, gl::Config& config) {
    raster.bind(true);
    shader.u_image = 0;
    array.bind(shader, vertices, config, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.index())));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
