#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

RenderSymbolLayer::RenderSymbolLayer(Immutable<style::SymbolLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Symbol, _impl) {
}

const style::SymbolLayer::Impl& RenderSymbolLayer::impl() const {
    return static_cast<const style::SymbolLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderSymbolLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> RenderSymbolLayer::createLayout(const BucketParameters& parameters,
                                                              const std::vector<const RenderLayer*>& group,
                                                              const GeometryTileLayer& layer,
                                                              GlyphDependencies& glyphDependencies,
                                                              IconDependencies& iconDependencies) const {
    return std::make_unique<SymbolLayout>(parameters,
                                          group,
                                          layer,
                                          iconDependencies,
                                          glyphDependencies);
}

void RenderSymbolLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transition(parameters, std::move(unevaluated));
}

void RenderSymbolLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    auto hasIconOpacity = evaluated.get<style::IconColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::IconHaloColor>().constantOr(Color::black()).a > 0;
    auto hasTextOpacity = evaluated.get<style::TextColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::TextHaloColor>().constantOr(Color::black()).a > 0;

    passes = ((evaluated.get<style::IconOpacity>().constantOr(1) > 0 && hasIconOpacity && iconSize > 0)
              || (evaluated.get<style::TextOpacity>().constantOr(1) > 0 && hasTextOpacity && textSize > 0))
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderSymbolLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

style::IconPaintProperties::PossiblyEvaluated RenderSymbolLayer::iconPaintProperties() const {
    return style::IconPaintProperties::PossiblyEvaluated {
            evaluated.get<style::IconOpacity>(),
            evaluated.get<style::IconColor>(),
            evaluated.get<style::IconHaloColor>(),
            evaluated.get<style::IconHaloWidth>(),
            evaluated.get<style::IconHaloBlur>(),
            evaluated.get<style::IconTranslate>(),
            evaluated.get<style::IconTranslateAnchor>()
    };
}

style::TextPaintProperties::PossiblyEvaluated RenderSymbolLayer::textPaintProperties() const {
    return style::TextPaintProperties::PossiblyEvaluated {
            evaluated.get<style::TextOpacity>(),
            evaluated.get<style::TextColor>(),
            evaluated.get<style::TextHaloColor>(),
            evaluated.get<style::TextHaloWidth>(),
            evaluated.get<style::TextHaloBlur>(),
            evaluated.get<style::TextTranslate>(),
            evaluated.get<style::TextTranslateAnchor>()
    };
}


style::SymbolPropertyValues RenderSymbolLayer::iconPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    return style::SymbolPropertyValues {
            layout_.get<style::IconRotationAlignment>(), // icon-pitch-alignment is not yet implemented; inherit the rotation alignment
            layout_.get<style::IconRotationAlignment>(),
            layout_.get<style::IconSize>(),
            evaluated.get<style::IconTranslate>(),
            evaluated.get<style::IconTranslateAnchor>(),
            iconSize,
            1.0f,
            evaluated.get<style::IconHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated.get<style::IconHaloWidth>().constantOr(1),
            evaluated.get<style::IconColor>().constantOr(Color::black()).a > 0,
            10.0f
    };
}

style::SymbolPropertyValues RenderSymbolLayer::textPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    // We hide line labels with viewport alignment as they move into the distance
    // because the approximations we use for drawing their glyphs get progressively worse
    // The "1.5" here means we start hiding them when the distance from the label
    // to the camera is 50% greater than the distance from the center of the map
    // to the camera. Depending on viewport properties, you might expect this to filter
    // the top third of the screen at pitch 60, and do almost nothing at pitch 45
    // "10" is effectively infinite at any pitch we support
    const bool limitMaxDistance =
        layout_.get<style::SymbolPlacement>() == style::SymbolPlacementType::Line
        && layout_.get<style::TextRotationAlignment>() == style::AlignmentType::Map
        && layout_.get<style::TextPitchAlignment>() == style::AlignmentType::Viewport;

    return style::SymbolPropertyValues {
            layout_.get<style::TextPitchAlignment>(),
            layout_.get<style::TextRotationAlignment>(),
            layout_.get<style::TextSize>(),
            evaluated.get<style::TextTranslate>(),
            evaluated.get<style::TextTranslateAnchor>(),
            textSize,
            24.0f,
            evaluated.get<style::TextHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated.get<style::TextHaloWidth>().constantOr(1),
            evaluated.get<style::TextColor>().constantOr(Color::black()).a > 0,
            limitMaxDistance ? 1.5f : 10.0f
    };
}

} // namespace mbgl
