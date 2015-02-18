#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/painter.hpp>

using namespace mbgl;

RasterBucket::RasterBucket(TexturePool& texturePool, const StyleLayoutRaster& layout_)
: layout(layout_),
  raster(texturePool) {
}

void RasterBucket::render(Painter &painter, util::ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix) {
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

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, GLuint texture_) {
    raster.bind(texture_);
    shader.u_image = 0;
    array.bind(shader, vertices, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
