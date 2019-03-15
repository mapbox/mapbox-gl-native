#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/background_program.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gfx/context.hpp>

namespace mbgl {

using namespace style;

RenderBackgroundLayer::RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl> _impl)
    : RenderLayer(std::move(_impl)),
      unevaluated(impl().paint.untransitioned()) {
}

const style::BackgroundLayer::Impl& RenderBackgroundLayer::impl() const {
    return static_cast<const style::BackgroundLayer::Impl&>(*baseImpl);
}

void RenderBackgroundLayer::transition(const TransitionParameters &parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    evaluated = unevaluated.evaluate(parameters);
    crossfade = parameters.getCrossfadeParameters();

    passes = evaluated.get<style::BackgroundOpacity>() > 0 ? RenderPass::Translucent
                                                           : RenderPass::None;
}

bool RenderBackgroundLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderBackgroundLayer::hasCrossfade() const {
    return crossfade.t != 1;
}

void RenderBackgroundLayer::render(PaintParameters& parameters, RenderSource*) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.

    const Properties<>::PossiblyEvaluated properties;
    const BackgroundProgram::Binders paintAttributeData(properties, 0);

    auto draw = [&](auto& program, auto&& uniformValues, const auto& textureBindings) {
        const auto allUniformValues = program.computeAllUniformValues(
            std::move(uniformValues),
            paintAttributeData,
            properties,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = program.computeAllAttributeBindings(
            parameters.staticData.tileVertexBuffer,
            paintAttributeData,
            properties
        );

        checkRenderability(parameters, program.activeBindingCount(allAttributeBindings));

        program.draw(
            parameters.context,
            gfx::Triangles(),
            parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
            gfx::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            gfx::CullFaceMode::disabled(),
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.tileTriangleSegments,
            allUniformValues,
            allAttributeBindings,
            textureBindings,
            getID()
        );
    };

    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA = parameters.imageManager.getPattern(evaluated.get<BackgroundPattern>().from);
        optional<ImagePosition> imagePosB = parameters.imageManager.getPattern(evaluated.get<BackgroundPattern>().to);

        if (!imagePosA || !imagePosB)
            return;

        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.getBackgroundLayerPrograms().backgroundPattern,
                BackgroundPatternProgram::layoutUniformValues(
                    parameters.matrixForTile(tileID),
                    evaluated.get<BackgroundOpacity>(),
                    parameters.imageManager.getPixelSize(),
                    *imagePosA,
                    *imagePosB,
                    crossfade,
                    tileID,
                    parameters.state
                ),
                BackgroundPatternProgram::TextureBindings{
                    textures::u_image::Value{ parameters.imageManager.textureBinding(parameters.context) },
                }
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.getBackgroundLayerPrograms().background,
                BackgroundProgram::LayoutUniformValues {
                    uniforms::u_matrix::Value( parameters.matrixForTile(tileID) ),
                    uniforms::u_color::Value( evaluated.get<BackgroundColor>() ),
                    uniforms::u_opacity::Value( evaluated.get<BackgroundOpacity>() ),
                },
                BackgroundProgram::TextureBindings{}
            );
        }
    }
}

optional<Color> RenderBackgroundLayer::getSolidBackground() const {
    if (!evaluated.get<BackgroundPattern>().from.empty()) {
        return nullopt;
    }

    return { evaluated.get<BackgroundColor>() * evaluated.get<BackgroundOpacity>() };
}

} // namespace mbgl
