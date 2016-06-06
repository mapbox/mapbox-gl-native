#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/raster_renderable.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(gl::TexturePool& texturePool)
    : renderable(std::make_unique<RasterRenderable>(texturePool)) {
}

RasterBucket::~RasterBucket() = default;

void RasterBucket::upload(gl::ObjectStore& store) {
    if (hasData()) {
        renderable->raster.upload(store);
    }

    uploaded = true;
}

void RasterBucket::render(Painter& painter,
                          const Layer& layer,
                          const UnwrappedTileID& tileID,
                          const mat4& matrix) {
    painter.renderRaster(*this, *layer.as<RasterLayer>(), tileID, matrix);
}

void RasterBucket::setImage(PremultipliedImage image) {
    renderable->raster.load(std::move(image));
}

void RasterBucket::drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, gl::ObjectStore& store) {
    renderable->raster.bind(true, store);
    array.bind(shader, vertices, BUFFER_OFFSET_0, store);
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index()));
}

bool RasterBucket::hasData() const {
    return renderable && renderable->raster.isLoaded();
}

bool RasterBucket::needsClipping() const {
    return false;
}

} // namespace mbgl
