#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/tile_cover.hpp>

namespace mbgl {

using namespace style;

void Painter::renderBackground(PaintParameters& parameters, const BackgroundLayer& layer) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.
    const BackgroundPaintProperties::Evaluated& properties = layer.impl->paint.evaluated;

    if (!properties.get<BackgroundPattern>().to.empty()) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(
            properties.get<BackgroundPattern>().from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(
            properties.get<BackgroundPattern>().to, SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB)
            return;

        spriteAtlas->bind(true, context, 0);

        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            parameters.programs.fillPattern.draw(
                context,
                gl::Triangles(),
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                FillPatternUniforms::values(
                    matrixForTile(tileID),
                    properties.get<BackgroundOpacity>(),
                    context.viewport.getCurrentValue().size,
                    *imagePosA,
                    *imagePosB,
                    properties.get<BackgroundPattern>(),
                    tileID,
                    state
                ),
                tileVertexBuffer,
                tileTriangleIndexBuffer,
                tileTriangleSegments
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            parameters.programs.fill.draw(
                context,
                gl::Triangles(),
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{ matrixForTile(tileID) },
                    uniforms::u_opacity::Value{ properties.get<BackgroundOpacity>() },
                    uniforms::u_color::Value{ properties.get<BackgroundColor>() },
                    uniforms::u_outline_color::Value{ properties.get<BackgroundColor>() },
                    uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
                },
                tileVertexBuffer,
                tileTriangleIndexBuffer,
                tileTriangleSegments
            );
        }
    }
}

} // namespace mbgl
