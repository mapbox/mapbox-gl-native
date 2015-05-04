#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

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

bool RasterBucket::setImage(const std::string &data) {
    return raster.load(data);
}

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array) {
    raster.bind(true);
    shader.u_image = 0;
    array.bind(shader, vertices, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
