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
    layout.cap.parse("line-cap", value);
    layout.join.parse("line-join", value);
    layout.miterLimit.parse("line-miter-limit", value);
    layout.roundLimit.parse("line-round-limit", value);
}

void LineLayer::parsePaints(const JSValue& layer) {
    paint.opacity.parse("line-opacity", layer);
    paint.color.parse("line-color", layer);
    paint.translate.parse("line-translate", layer);
    paint.translateAnchor.parse("line-translate-anchor", layer);
    paint.width.parse("line-width", layer);
    paint.gapWidth.parse("line-gap-width", layer);
    paint.offset.parse("line-offset", layer);
    paint.blur.parse("line-blur", layer);
    paint.dasharray.parse("line-dasharray", layer);
    paint.pattern.parse("line-pattern", layer);
}

void LineLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.opacity.cascade(parameters);
    paint.color.cascade(parameters);
    paint.translate.cascade(parameters);
    paint.translateAnchor.cascade(parameters);
    paint.width.cascade(parameters);
    paint.gapWidth.cascade(parameters);
    paint.offset.cascade(parameters);
    paint.blur.cascade(parameters);
    paint.dasharray.cascade(parameters);
    paint.pattern.cascade(parameters);
}

bool LineLayer::recalculate(const StyleCalculationParameters& parameters) {
    // for scaling dasharrays
    StyleCalculationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    paint.width.calculate(dashArrayParams);
    paint.dashLineWidth = paint.width;

    bool hasTransitions = false;

    hasTransitions |= paint.opacity.calculate(parameters);
    hasTransitions |= paint.color.calculate(parameters);
    hasTransitions |= paint.translate.calculate(parameters);
    hasTransitions |= paint.translateAnchor.calculate(parameters);
    hasTransitions |= paint.width.calculate(parameters);
    hasTransitions |= paint.gapWidth.calculate(parameters);
    hasTransitions |= paint.offset.calculate(parameters);
    hasTransitions |= paint.blur.calculate(parameters);
    hasTransitions |= paint.dasharray.calculate(parameters);
    hasTransitions |= paint.pattern.calculate(parameters);

    passes = paint.isVisible() ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> LineLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<LineBucket>(parameters.tileID.overscaling);

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.z);
    bucket->layout.cap.calculate(p);
    bucket->layout.join.calculate(p);
    bucket->layout.miterLimit.calculate(p);
    bucket->layout.roundLimit.calculate(p);

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(getGeometries(feature));
    });

    return std::move(bucket);
}

} // namespace mbgl
