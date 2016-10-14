#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/renderer/terrain_bucket.hpp>
#include <mbgl/style/layers/terrain_layer.hpp>
#include <mbgl/style/layers/terrain_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/offscreen_texture.hpp>

namespace mbgl {

using namespace style;

void Painter::renderTerrain(PaintParameters& parameters,
                           TerrainBucket& bucket,
                           const TerrainLayer& layer,
                           const RenderTile& tile) {
    if (pass != RenderPass::Translucent) return;

    if (!bucket.prepared) {
        OffscreenTexture view(context, { 256, 256 });
        view.bind();

        context.bindTexture(*bucket.dem, 0, gl::TextureFilter::Linear, gl::TextureMipMap::Yes);

        mat4 mat;
        matrix::ortho(mat, 0, util::EXTENT, -util::EXTENT, 0, 0, 1);
        matrix::translate(mat, mat, 0, -util::EXTENT, 0);

        context.clear(Color::red(), {}, {});
        context.draw({
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            gl::ColorMode::unblended(),
            parameters.shaders.terrainPrepare,
            TerrainPrepareUniforms::Values {
                mat,
                0,
                { {{ 512, 512 }} },
                tile.id.canonical.z
            },
            gl::Unindexed<gl::TriangleStrip>(rasterVertexBuffer)
        });

        bucket.texture = std::move(view.getTexture());
        bucket.prepared = true;

        parameters.view.bind();
    }

    const TerrainPaintProperties& properties = layer.impl->paint;

        double azimuth = (-properties.terrainIlluminationDirection - 90) * util::DEG2RAD;
        if (properties.terrainIlluminationAlignment == style::AlignmentType::Viewport) {
            azimuth -= state.getAngle();
        }

    context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);

    context.draw({
        depthModeForSublayer(0, gl::DepthMode::ReadOnly),
        stencilModeForClipping(tile.clip),
        colorModeForRenderPass(),
        parameters.shaders.terrain,
        TerrainUniforms::Values {
            tile.matrix,
            0, // image
            8, // mode
            { {{ 256, 256 }} },
            tile.id.canonical.z,
            azimuth,
            60 * util::DEG2RAD, // zenith
            0, // mipmap
            properties.terrainExaggeration.value,
            properties.terrainShadowColor.value,
            properties.terrainHighlightColor.value,
            properties.terrainAccentColor.value
        },
        gl::Unindexed<gl::TriangleStrip>(rasterVertexBuffer)
    });
}

} // namespace mbgl
