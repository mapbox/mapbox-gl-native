#include <mbgl/layer/line_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/get_geometries.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> LineLayer::clone() const {
    return std::make_unique<LineLayer>(*this);
}

void LineLayer::parseLayout(const JSValue& value) {
    layout.lineCap.parse("line-cap", value);
    layout.lineJoin.parse("line-join", value);
    layout.lineMiterLimit.parse("line-miter-limit", value);
    layout.lineRoundLimit.parse("line-round-limit", value);
}

void LineLayer::parsePaints(const JSValue& layer) {
    paint.lineOpacity.parse("line-opacity", layer);
    paint.lineColor.parse("line-color", layer);
    paint.lineTranslate.parse("line-translate", layer);
    paint.lineTranslateAnchor.parse("line-translate-anchor", layer);
    paint.lineWidth.parse("line-width", layer);
    paint.lineGapWidth.parse("line-gap-width", layer);
    paint.lineOffset.parse("line-offset", layer);
    paint.lineBlur.parse("line-blur", layer);
    paint.lineDasharray.parse("line-dasharray", layer);
    paint.linePattern.parse("line-pattern", layer);
}

void LineLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.lineOpacity.cascade(parameters);
    paint.lineColor.cascade(parameters);
    paint.lineTranslate.cascade(parameters);
    paint.lineTranslateAnchor.cascade(parameters);
    paint.lineWidth.cascade(parameters);
    paint.lineGapWidth.cascade(parameters);
    paint.lineOffset.cascade(parameters);
    paint.lineBlur.cascade(parameters);
    paint.lineDasharray.cascade(parameters);
    paint.linePattern.cascade(parameters);
}

bool LineLayer::recalculate(const StyleCalculationParameters& parameters) {
    // for scaling dasharrays
    StyleCalculationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    paint.lineWidth.calculate(dashArrayParams);
    paint.dashLineWidth = paint.lineWidth;

    bool hasTransitions = false;

    hasTransitions |= paint.lineOpacity.calculate(parameters);
    hasTransitions |= paint.lineColor.calculate(parameters);
    hasTransitions |= paint.lineTranslate.calculate(parameters);
    hasTransitions |= paint.lineTranslateAnchor.calculate(parameters);
    hasTransitions |= paint.lineWidth.calculate(parameters);
    hasTransitions |= paint.lineGapWidth.calculate(parameters);
    hasTransitions |= paint.lineOffset.calculate(parameters);
    hasTransitions |= paint.lineBlur.calculate(parameters);
    hasTransitions |= paint.lineDasharray.calculate(parameters);
    hasTransitions |= paint.linePattern.calculate(parameters);

    passes = paint.isVisible() ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> LineLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<LineBucket>(parameters.tileID.overscaleFactor());

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.z);
    bucket->layout.lineCap.calculate(p);
    bucket->layout.lineJoin.calculate(p);
    bucket->layout.lineMiterLimit.calculate(p);
    bucket->layout.lineRoundLimit.calculate(p);

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(getGeometries(feature));
    });

    return std::move(bucket);
}

} // namespace mbgl
