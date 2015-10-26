#include <mbgl/layer/line_layer.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/map/tile_id.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> LineLayer::clone() const {
    std::unique_ptr<LineLayer> result = std::make_unique<LineLayer>();
    result->copy(*this);
    result->layout = layout;
    result->paints.paints = paints.paints;
    return std::move(result);
}

void LineLayer::parseLayout(const JSVal& value) {
    layout.cap.parse("line-cap", value);
    layout.join.parse("line-join", value);
    layout.miterLimit.parse("line-miter-limit", value);
    layout.roundLimit.parse("line-round-limit", value);
}

void LineLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("line-opacity", PropertyKey::LineOpacity, paint, value);
        parseProperty<PropertyTransition>("line-opacity-transition", PropertyKey::LineOpacity, paint, value);
        parseProperty<Function<Color>>("line-color", PropertyKey::LineColor, paint, value);
        parseProperty<PropertyTransition>("line-color-transition", PropertyKey::LineColor, paint, value);
        parseProperty<Function<std::array<float,2>>>("line-translate", PropertyKey::LineTranslate, paint, value);
        parseProperty<PropertyTransition>("line-translate-transition", PropertyKey::LineTranslate, paint, value);
        parseProperty<Function<TranslateAnchorType>>("line-translate-anchor", PropertyKey::LineTranslateAnchor, paint, value);
        parseProperty<Function<float>>("line-width", PropertyKey::LineWidth, paint, value);
        parseProperty<PropertyTransition>("line-width-transition", PropertyKey::LineWidth, paint, value);
        parseProperty<Function<float>>("line-gap-width", PropertyKey::LineGapWidth, paint, value);
        parseProperty<PropertyTransition>("line-gap-width-transition", PropertyKey::LineGapWidth, paint, value);
        parseProperty<Function<float>>("line-blur", PropertyKey::LineBlur, paint, value);
        parseProperty<PropertyTransition>("line-blur-transition", PropertyKey::LineBlur, paint, value);
        parseProperty<Function<Faded<std::vector<float>>>>("line-dasharray", PropertyKey::LineDashArray, paint, value);
        parseProperty<Function<Faded<std::string>>>("line-pattern", PropertyKey::LineImage, paint, value);
    });
}

void LineLayer::cascade(const StyleCascadeParameters& parameters) {
    paints.cascade(parameters);
}

bool LineLayer::hasTransitions() const {
    return paints.hasTransitions();
}

void LineLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::LineOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::LineColor, properties.color, parameters);
    paints.calculateTransitioned(PropertyKey::LineTranslate, properties.translate, parameters);
    paints.calculate(PropertyKey::LineTranslateAnchor, properties.translateAnchor, parameters);
    paints.calculateTransitioned(PropertyKey::LineWidth, properties.width, parameters);
    paints.calculateTransitioned(PropertyKey::LineGapWidth, properties.gap_width, parameters);
    paints.calculateTransitioned(PropertyKey::LineBlur, properties.blur, parameters);
    paints.calculate(PropertyKey::LineDashArray, properties.dash_array, parameters);
    paints.calculate(PropertyKey::LineImage, properties.image, parameters);

    // for scaling dasharrays
    StyleCalculationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    paints.calculate(PropertyKey::LineWidth, properties.dash_line_width, dashArrayParams);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

std::unique_ptr<Bucket> LineLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<LineBucket>();

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.z);
    bucket->layout.cap.calculate(p);
    bucket->layout.join.calculate(p);
    bucket->layout.miterLimit.calculate(p);
    bucket->layout.roundLimit.calculate(p);

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(feature.getGeometries());
    });

    return std::move(bucket);
}

}
