#include <mbgl/layer/line_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/get_geometries.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

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
    dashLineWidth = paint.lineWidth;

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

    passes = (paint.lineOpacity > 0 && paint.lineColor.value[3] > 0 && paint.lineWidth > 0)
        ? RenderPass::Translucent : RenderPass::None;

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

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = getGeometries(feature);
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}


float LineLayer::getLineWidth() const {
    if (paint.lineGapWidth > 0) {
        return paint.lineGapWidth + 2 * paint.lineWidth;
    } else {
        return paint.lineWidth;
    }
}

optional<GeometryCollection> offsetLine(const GeometryCollection& rings, const float offset) {
    if (offset == 0) return {};

    GeometryCollection newRings;
    vec2<double> zero(0, 0);
    for (auto& ring : rings) {
        newRings.emplace_back();
        auto& newRing = newRings.back();

        for (auto i = ring.begin(); i != ring.end(); i++) {
            auto& p = *i;

            auto aToB = i == ring.begin() ?
                zero :
                util::perp(util::unit(vec2<double>(p - *(i - 1))));
            auto bToC = i + 1 == ring.end() ?
                zero :
                util::perp(util::unit(vec2<double>(*(i + 1) - p)));
            auto extrude = util::unit(aToB + bToC);

            const double cosHalfAngle = extrude.x * bToC.x + extrude.y * bToC.y;
            extrude *= (1.0 / cosHalfAngle);

            newRing.push_back((extrude * offset) + p);
        }
    }

    return newRings;
}

float LineLayer::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.lineTranslate;
    return getLineWidth() / 2.0 + std::abs(paint.lineOffset) + util::length(translate[0], translate[1]);
}

bool LineLayer::queryIntersectsGeometry(
        const GeometryCollection& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    const float halfWidth = getLineWidth() / 2.0 * pixelsToTileUnits;

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.lineTranslate, paint.lineTranslateAnchor, bearing, pixelsToTileUnits);
    auto offsetGeometry = offsetLine(geometry, paint.lineOffset * pixelsToTileUnits);

    return util::multiPolygonIntersectsBufferedMultiLine(
            translatedQueryGeometry.value_or(queryGeometry),
            offsetGeometry.value_or(geometry),
            halfWidth);
}

} // namespace mbgl
