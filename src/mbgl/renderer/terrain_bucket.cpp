#include <mbgl/renderer/terrain_bucket.hpp>
#include <mbgl/style/layers/terrain_layer.hpp>
#include <mbgl/shader/terrain_shader.hpp>
#include <mbgl/shader/terrain_prepare_shader.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

TerrainBucket::TerrainBucket(PremultipliedImage&&) {
    // TODO
}

TerrainBucket::TerrainBucket(DEMPyramid&& pyramid_) : pyramid(std::move(pyramid_)) {
}

void TerrainBucket::upload(gl::Context& context) {
    if (!pyramid.levels.empty()) {
        dem = context.createTexture(pyramid.levels.front().image);
        for (size_t l = 1; l < pyramid.levels.size(); l++) {
            auto& image = pyramid.levels[l].image;
            MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, l, GL_RGBA, image.size.width, image.size.height, 0,
                                          GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));
        }
    }

    uploaded = true;
}

void TerrainBucket::render(Painter& painter,
                           PaintParameters& parameters,
                           const Layer& layer,
                           const RenderTile& tile) {
    painter.renderTerrain(parameters, *this, *layer.as<TerrainLayer>(), tile);
}

bool TerrainBucket::hasData() const {
    return true;
}

} // namespace mbgl
