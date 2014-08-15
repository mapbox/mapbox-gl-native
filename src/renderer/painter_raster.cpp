#include <mbgl/renderer/painter.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/transform.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    if (pass != RenderPass::Translucent) return;

    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();

    if (layer_desc->layers) {

        if (!bucket.texture.getTexture()) {

            bucket.texture.bindFramebuffer(state);

            preparePrerender(bucket);

            // Save the current viewport dimension so that we can restore it later.
            const std::array<uint16_t, 2> viewport = state.viewport();

            glViewport(0, 0, bucket.properties.size, bucket.properties.size);

            const int buffer = bucket.properties.buffer * 4096.0f;

            const mat4 preMatrix = [&]{
                mat4 vtxMatrix;
                matrix::ortho(vtxMatrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
                matrix::translate(vtxMatrix, vtxMatrix, 0, -4096, 0);
                return vtxMatrix;
            }();

            // call updateTiles to get parsed data for sublayers
            map.updateTiles();

            for (const std::shared_ptr<StyleLayer> &layer : layer_desc->layers->layers) {
                setOpaque();
                map.renderLayer(layer, RenderPass::Opaque, &id, &preMatrix);
                setTranslucent();
                map.renderLayer(layer, RenderPass::Translucent, &id, &preMatrix);
            }

            if (bucket.properties.blur > 0) {
                bucket.texture.blur(*this, bucket.properties.blur);
            }

            bucket.texture.unbindFramebuffer();

            state.depthTest(true);
            state.stencilTest(true);

            // Restore the viewport.
            state.viewport(viewport);

        }

        renderPrerenderedTexture(bucket, matrix, properties);

    }

    // Only draw non-prerendered raster here
    if (bucket.hasData()) {
        state.depthMask(false);

        state.useProgram(rasterShader->program);
        rasterShader->setMatrix(matrix);
        rasterShader->setBuffer(0);
        rasterShader->setOpacity(properties.opacity);
        rasterShader->setBrightness(properties.brightness[0], properties.brightness[1]);
        rasterShader->setSaturation(properties.saturation);
        rasterShader->setContrast(properties.contrast);
        rasterShader->setSpin(spinWeights(properties.hue_rotate));

        state.depthRange({{ strata + strata_epsilon, 1.0f }});

        bucket.drawRaster(state, *rasterShader, tileStencilBuffer, coveringRasterArray);

        state.depthMask(true);
    }

}

std::array<float, 3> Painter::spinWeights(float spin) {
    spin *= M_PI / 180;
    float s = std::sin(spin);
    float c = std::cos(spin);
    std::array<float, 3> spin_weights = {{
        (2 * c + 1) / 3,
        (-std::sqrt(3.0f) * s - c + 1) / 3,
        (std::sqrt(3.0f) * s - c + 1) / 3
    }};
    return spin_weights;
}
