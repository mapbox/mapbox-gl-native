#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

RasterBucket::RasterBucket(PremultipliedImage&& image_) {
    image = std::make_shared<PremultipliedImage>(std::move(image_));
}

RasterBucket::RasterBucket(std::shared_ptr<PremultipliedImage> image_): image(image_) {

}

void RasterBucket::upload(gl::Context& context) {
    if (!hasData()) {
        return;
    }
    if (!texture) {
        texture = context.createTexture(*image);
    }
    if (!vertices.empty()) {
        vertexBuffer = context.createVertexBuffer(std::move(vertices));
        indexBuffer = context.createIndexBuffer(std::move(indices));
    }
    uploaded = true;
}

void RasterBucket::clear() {
    vertexBuffer = {};
    indexBuffer = {};
    segments.clear();
    vertices.clear();
    indices.clear();

    uploaded = false;
}

void RasterBucket::setImage(std::shared_ptr<PremultipliedImage> image_) {
    image = std::move(image_);
    texture = {};
    uploaded = false;
}

bool RasterBucket::hasData() const {
    return !!image;
}

} // namespace mbgl
