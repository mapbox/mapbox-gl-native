#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>

namespace mbgl {

using namespace style;

namespace {

inline const BackgroundLayer::Impl& impl_cast(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == BackgroundLayer::Impl::staticTypeInfo());
    return static_cast<const style::BackgroundLayer::Impl&>(*impl);
}

} // namespace

RenderBackgroundLayer::RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl> _impl)
    : RenderLayer(makeMutable<BackgroundLayerProperties>(std::move(_impl))),
      unevaluated(impl_cast(baseImpl).paint.untransitioned()) {}

RenderBackgroundLayer::~RenderBackgroundLayer() = default;

void RenderBackgroundLayer::transition(const TransitionParameters &parameters) {
    unevaluated = impl_cast(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    auto properties = makeMutable<BackgroundLayerProperties>(
        staticImmutableCast<BackgroundLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));

    passes = properties->evaluated.get<style::BackgroundOpacity>() == 0.0f
        ? RenderPass::None
        : (!unevaluated.get<style::BackgroundPattern>().isUndefined()
           || properties->evaluated.get<style::BackgroundOpacity>() < 1.0f
           || properties->evaluated.get<style::BackgroundColor>().a < 1.0f)
        ? RenderPass::Translucent
        // Supply both - evaluated based on opaquePassCutoff in render().
        : RenderPass::Opaque | RenderPass::Translucent;
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

    auto draw = [&](auto& program, auto&& uniformValues, const auto& textureBindings, const uint32_t id) {
        const auto allUniformValues =
            program.computeAllUniformValues(std::forward<decltype(uniformValues)>(uniformValues),
                                            paintAttributeData,
                                            properties,
                                            parameters.state.getZoom());
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
            parameters.depthModeForSublayer(
                0,
                parameters.pass == RenderPass::Opaque ? gfx::DepthMaskType::ReadWrite : gfx::DepthMaskType::ReadOnly),
            gfx::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            gfx::CullFaceMode::disabled(),
            *parameters.staticData.quadTriangleIndexBuffer,
            segments,
            allUniformValues,
            allAttributeBindings,
            textureBindings,
            util::toString(id));
    };

    if (segments.empty()) {
        segments = RenderStaticData::tileTriangleSegments();
    }

    const auto& evaluated = static_cast<const BackgroundLayerProperties&>(*evaluatedProperties).evaluated;
    const auto& crossfade = static_cast<const BackgroundLayerProperties&>(*evaluatedProperties).crossfade;
    if (!evaluated.get<BackgroundPattern>().to.empty()) {
        optional<ImagePosition> imagePosA =
            parameters.patternAtlas.getPattern(evaluated.get<BackgroundPattern>().from.id());
        optional<ImagePosition> imagePosB =
            parameters.patternAtlas.getPattern(evaluated.get<BackgroundPattern>().to.id());

        if (!imagePosA || !imagePosB)
            return;

        uint32_t i = 0;
        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            const UnwrappedTileID unwrappedTileID = tileID.toUnwrapped();
            draw(parameters.programs.getBackgroundLayerPrograms().backgroundPattern,
                 BackgroundPatternProgram::layoutUniformValues(parameters.matrixForTile(unwrappedTileID),
                                                               evaluated.get<BackgroundOpacity>(),
                                                               parameters.patternAtlas.getPixelSize(),
                                                               *imagePosA,
                                                               *imagePosB,
                                                               crossfade,
                                                               unwrappedTileID,
                                                               parameters.state),
                 BackgroundPatternProgram::TextureBindings{
                     textures::image::Value{parameters.patternAtlas.textureBinding()},
                 },
                 i++);
        }
    } else {
        auto backgroundRenderPass = (evaluated.get<BackgroundColor>().a >= 1.0f
            && evaluated.get<BackgroundOpacity>() >= 1.0f
            && parameters.currentLayer >= parameters.opaquePassCutoff) ? RenderPass::Opaque : RenderPass::Translucent;
        if (parameters.pass != backgroundRenderPass) {
            return;
        }
        uint32_t i = 0;
        for (const auto& tileID : util::tileCover(parameters.state, parameters.state.getIntegerZoom())) {
            draw(parameters.programs.getBackgroundLayerPrograms().background,
                 BackgroundProgram::LayoutUniformValues{
                     uniforms::matrix::Value(parameters.matrixForTile(tileID.toUnwrapped())),
                     uniforms::color::Value(evaluated.get<BackgroundColor>()),
                     uniforms::opacity::Value(evaluated.get<BackgroundOpacity>()),
                 },
                 BackgroundProgram::TextureBindings{},
                 i++);
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
        addPatternIfNeeded(evaluated.get<BackgroundPattern>().from.id(), params);
        addPatternIfNeeded(evaluated.get<BackgroundPattern>().to.id(), params);
    }
}

} // namespace mbgl
