#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/util/tile_cover.hpp>

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

void RenderBackgroundLayer::render(Painter& painter, PaintParameters& parameters, RenderSource*) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.

    style::FillPaintProperties::PossiblyEvaluated properties;
    properties.get<FillPattern>() = evaluated.get<BackgroundPattern>();
    properties.get<FillOpacity>() = { evaluated.get<BackgroundOpacity>() };
    properties.get<FillColor>() = { evaluated.get<BackgroundColor>() };

    const FillProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA = painter.imageManager->getPattern(evaluated.get<BackgroundPattern>().from);
        optional<ImagePosition> imagePosB = painter.imageManager->getPattern(evaluated.get<BackgroundPattern>().to);

        if (!imagePosA || !imagePosB)
            return;

        painter.imageManager->bind(painter.context, 0);

        for (const auto& tileID : util::tileCover(painter.state, painter.state.getIntegerZoom())) {
            parameters.programs.fillPattern.get(properties).draw(
                painter.context,
                gl::Triangles(),
                painter.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                painter.colorModeForRenderPass(),
                FillPatternUniforms::values(
                    painter.matrixForTile(tileID),
                    painter.context.viewport.getCurrentValue().size,
                    painter.imageManager->getPixelSize(),
                    *imagePosA,
                    *imagePosB,
                    evaluated.get<BackgroundPattern>(),
                    tileID,
                    painter.state
                ),
                painter.tileVertexBuffer,
                painter.quadTriangleIndexBuffer,
                painter.tileTriangleSegments,
                paintAttibuteData,
                properties,
                painter.state.getZoom(),
                getID()
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(painter.state, painter.state.getIntegerZoom())) {
            parameters.programs.fill.get(properties).draw(
                painter.context,
                gl::Triangles(),
                painter.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                painter.colorModeForRenderPass(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{ painter.matrixForTile(tileID) },
                    uniforms::u_world::Value{ painter.context.viewport.getCurrentValue().size },
                },
                painter.tileVertexBuffer,
                painter.quadTriangleIndexBuffer,
                painter.tileTriangleSegments,
                paintAttibuteData,
                properties,
                painter.state.getZoom(),
                getID()
            );
        }
    }
}

} // namespace mbgl
