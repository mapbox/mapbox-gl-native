#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(PremultipliedImage&& image_) : image(std::move(image_)) {
}

void RasterBucket::upload(gl::Context& context) {
    texture = context.createTexture(image);
    image = {};
    uploaded = true;
}

void RasterBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const Layer& layer,
                          const RenderTile& tile) {
    painter.renderRaster(parameters, *this, *layer.as<RasterLayer>(), tile);
}

void RasterBucket::drawRaster(RasterShader& shader,
                              gl::VertexBuffer<RasterVertex>& vertices,
                              gl::VertexArrayObject& array,
                              gl::Context& context) {
    assert(texture);
    context.bindTexture(*texture, 0, gl::TextureFilter::Linear);
    context.bindTexture(*texture, 1, gl::TextureFilter::Linear);
    array.bind(shader, vertices, BUFFER_OFFSET_0, context);
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.vertexCount)));
}

bool RasterBucket::hasData() const {
    return true;
}

bool RasterBucket::needsClipping() const {
    return false;
}

} // namespace mbgl
