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

namespace mbgl {

using namespace style;

RenderBackgroundLayer::RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Background, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::BackgroundLayer::Impl& RenderBackgroundLayer::impl() const {
    return static_cast<const style::BackgroundLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderBackgroundLayer::createBucket(const BucketParameters &,
                                                            const std::vector<const RenderLayer *> &) const {
    assert(false);
    return nullptr;
}

void RenderBackgroundLayer::transition(const TransitionParameters &parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::BackgroundOpacity>() > 0 ? RenderPass::Translucent
                                                           : RenderPass::None;
}

bool RenderBackgroundLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderBackgroundLayer::render(PaintParameters& parameters, RenderSource*) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.

    const Properties<>::PossiblyEvaluated properties;
    const BackgroundProgram::PaintPropertyBinders paintAttributeData(properties, 0);

    auto draw = [&](auto& program, auto&& uniformValues) {
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
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.tileTriangleSegments,
            allUniformValues,
            allAttributeBindings,
            getID()
        );
    };

    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA = parameters.imageManager.getPattern(evaluated.get<BackgroundPattern>().from);
        optional<ImagePosition> imagePosB = parameters.imageManager.getPattern(evaluated.get<BackgroundPattern>().to);

        if (!imagePosA || !imagePosB)
            return;

        parameters.imageManager.bind(parameters.context, 0);

        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.backgroundPattern,
                BackgroundPatternUniforms::values(
                    parameters.matrixForTile(tileID),
                    evaluated.get<BackgroundOpacity>(),
                    parameters.imageManager.getPixelSize(),
                    *imagePosA,
                    *imagePosB,
                    evaluated.get<BackgroundPattern>(),
                    tileID,
                    parameters.state
                )
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.background,
                BackgroundProgram::UniformValues {
                    uniforms::u_matrix::Value{ parameters.matrixForTile(tileID) },
                    uniforms::u_color::Value{ evaluated.get<BackgroundColor>() },
                    uniforms::u_opacity::Value{ evaluated.get<BackgroundOpacity>() },
                }
            );
        }
    }
}

} // namespace mbgl
