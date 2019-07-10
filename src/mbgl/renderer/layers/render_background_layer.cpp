#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/background_program.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>

namespace mbgl {

using namespace style;

inline const BackgroundLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    return static_cast<const style::BackgroundLayer::Impl&>(*impl);
}

RenderBackgroundLayer::RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl> _impl)
    : RenderLayer(makeMutable<BackgroundLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderBackgroundLayer::~RenderBackgroundLayer() = default;

void RenderBackgroundLayer::transition(const TransitionParameters &parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    auto properties = makeMutable<BackgroundLayerProperties>(
        staticImmutableCast<BackgroundLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));

    passes = properties->evaluated.get<style::BackgroundOpacity>() > 0 ? RenderPass::Translucent
                                                                       : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderBackgroundLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderBackgroundLayer::hasCrossfade() const {
    return getCrossfade<BackgroundLayerProperties>(evaluatedProperties).t != 1;
}

void RenderBackgroundLayer::render(PaintParameters& parameters) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.

    const Properties<>::PossiblyEvaluated properties;
    const BackgroundProgram::Binders paintAttributeData(properties, 0);

    auto draw = [&](auto& program, auto&& uniformValues, const auto& textureBindings, const UnwrappedTileID& id) {
        const auto allUniformValues = program.computeAllUniformValues(
            std::move(uniformValues),
            paintAttributeData,
            properties,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = program.computeAllAttributeBindings(
            *parameters.staticData.tileVertexBuffer,
            paintAttributeData,
            properties
        );

        checkRenderability(parameters, program.activeBindingCount(allAttributeBindings));

        program.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Triangles(),
            parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
            gfx::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            gfx::CullFaceMode::disabled(),
            *parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.tileTriangleSegments,
            allUniformValues,
            allAttributeBindings,
            textureBindings,
            getID() + "/" + util::toString(id)
        );
    };
    const auto& evaluated = static_cast<const BackgroundLayerProperties&>(*evaluatedProperties).evaluated;
    const auto& crossfade = static_cast<const BackgroundLayerProperties&>(*evaluatedProperties).crossfade;
    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA = parameters.patternAtlas.getPattern(evaluated.get<BackgroundPattern>().from);
        optional<ImagePosition> imagePosB = parameters.patternAtlas.getPattern(evaluated.get<BackgroundPattern>().to);

        if (!imagePosA || !imagePosB)
            return;

        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.getBackgroundLayerPrograms().backgroundPattern,
                BackgroundPatternProgram::layoutUniformValues(
                    parameters.matrixForTile(tileID),
                    evaluated.get<BackgroundOpacity>(),
                    parameters.patternAtlas.getPixelSize(),
                    *imagePosA,
                    *imagePosB,
                    crossfade,
                    tileID,
                    parameters.state
                ),
                BackgroundPatternProgram::TextureBindings{
                    textures::image::Value{ parameters.patternAtlas.textureBinding() },
                },
                tileID
            );
        }
    } else {
        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(
                parameters.programs.getBackgroundLayerPrograms().background,
                BackgroundProgram::LayoutUniformValues {
                    uniforms::matrix::Value( parameters.matrixForTile(tileID) ),
                    uniforms::color::Value( evaluated.get<BackgroundColor>() ),
                    uniforms::opacity::Value( evaluated.get<BackgroundOpacity>() ),
                },
                BackgroundProgram::TextureBindings{},
                tileID
            );
        }
    }
}

optional<Color> RenderBackgroundLayer::getSolidBackground() const {
    const auto& evaluated = getEvaluated<BackgroundLayerProperties>(evaluatedProperties);
    if (!evaluated.get<BackgroundPattern>().from.empty() || evaluated.get<style::BackgroundOpacity>() <= 0.0f) {
        return nullopt;
    }

    return { evaluated.get<BackgroundColor>() * evaluated.get<BackgroundOpacity>() };
}

namespace {
void addPatternIfNeeded(const std::string& id, const LayerPrepareParameters& params) {
    if (!params.patternAtlas.getPattern(id)) {
        if (auto* image = params.imageManager.getImage(id)) {
            params.patternAtlas.addPattern(*image);
        }
    }  
}
} // namespace

void RenderBackgroundLayer::prepare(const LayerPrepareParameters& params) {
    const auto& evaluated = getEvaluated<BackgroundLayerProperties>(evaluatedProperties);
    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        // Ensures that the pattern bitmap gets copied to atlas bitmap. 
        // Atlas bitmap is uploaded to atlas texture in upload.
        addPatternIfNeeded(evaluated.get<BackgroundPattern>().from, params);
        addPatternIfNeeded(evaluated.get<BackgroundPattern>().to, params);
    }
}

} // namespace mbgl
