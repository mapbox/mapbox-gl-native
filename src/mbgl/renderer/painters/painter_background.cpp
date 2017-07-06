#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/util/tile_cover.hpp>

namespace mbgl {

using namespace style;

void Painter::renderBackground(PaintParameters& parameters, const RenderBackgroundLayer& layer) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.
    const BackgroundPaintProperties::PossiblyEvaluated& background = layer.evaluated;

    style::FillPaintProperties::PossiblyEvaluated properties;
    properties.get<FillPattern>() = background.get<BackgroundPattern>();
    properties.get<FillOpacity>() = { background.get<BackgroundOpacity>() };
    properties.get<FillColor>() = { background.get<BackgroundColor>() };

    const FillProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    if (!background.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA = imageManager->getPattern(background.get<BackgroundPattern>().from);
        optional<ImagePosition> imagePosB = imageManager->getPattern(background.get<BackgroundPattern>().to);

        if (!imagePosA || !imagePosB)
            return;

        imageManager->bind(context, 0);

        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            parameters.programs.fillPattern.get(properties).draw(
                context,
                gl::Triangles(),
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                FillPatternUniforms::values(
                    matrixForTile(tileID),
                    context.viewport.getCurrentValue().size,
                    imageManager->getPixelSize(),
                    *imagePosA,
                    *imagePosB,
                    background.get<BackgroundPattern>(),
                    tileID,
                    state
                ),
                fillDrawable.vertices,
                fillDrawable.indices,
                fillDrawable.segments,
                paintAttibuteData,
                properties,
                state.getZoom()
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            parameters.programs.fill.get(properties).draw(
                context,
                gl::Triangles(),
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{ matrixForTile(tileID) },
                    uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
                },
                fillDrawable.vertices,
                fillDrawable.indices,
                fillDrawable.segments,
                paintAttibuteData,
                properties,
                state.getZoom()
            );
        }
    }
}

} // namespace mbgl
