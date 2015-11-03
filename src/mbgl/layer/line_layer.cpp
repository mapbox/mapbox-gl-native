#include <mbgl/layer/line_layer.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/map/tile_id.hpp>

namespace mbgl {

void LineLayer::parseLayout(const JSVal& value) {
    parseProperty<Function<CapType>>("line-cap", PropertyKey::LineCap, layout, value);
    parseProperty<Function<JoinType>>("line-join", PropertyKey::LineJoin, layout, value);
    parseProperty<Function<float>>("line-miter-limit", PropertyKey::LineMiterLimit, layout, value);
    parseProperty<Function<float>>("line-round-limit", PropertyKey::LineRoundLimit, layout, value);
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
        parseProperty<Function<Faded<std::vector<float>>>>("line-dasharray", PropertyKey::LineDashArray, paint, value, "line-dasharray-transition");
        parseProperty<Function<Faded<std::string>>>("line-pattern", PropertyKey::LineImage, paint, value, "line-pattern-transition");
    });
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

    const float z = parameters.tileID.z;

    layout.calculate(PropertyKey::LineCap, bucket->layout.cap, z);
    layout.calculate(PropertyKey::LineJoin, bucket->layout.join, z);
    layout.calculate(PropertyKey::LineMiterLimit, bucket->layout.miter_limit, z);
    layout.calculate(PropertyKey::LineRoundLimit, bucket->layout.round_limit, z);

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(feature.getGeometries());
    });

    return std::move(bucket);
}

}
