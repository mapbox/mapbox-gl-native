#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/renderer/painter.hpp>

using namespace llmr;

RasterBucket::RasterBucket(const std::shared_ptr<Texturepool> &texturepool)
    : raster(texturepool) {
}

void RasterBucket::render(Painter &painter, const std::string &layer_name, const Tile::ID &id) {
    painter.renderRaster(*this, layer_name, id);
}

bool RasterBucket::setImage(const std::string &data) {
    return raster.load(data);
}

void RasterBucket::drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array) {
    raster.bind(true);
    shader.setImage(0);
    array.bind(shader, vertices, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index());
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
