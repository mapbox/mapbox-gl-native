#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace style {

void LineLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool LineLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    // for scaling dasharrays
    PropertyEvaluationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    dashLineWidth = paint.evaluate<LineWidth>(dashArrayParams);

    paint.evaluate(parameters);

    passes = (paint.evaluated.get<LineOpacity>().constantOr(1.0) > 0
           && paint.evaluated.get<LineColor>().constantOr(Color::black()).a > 0
           && paint.evaluated.get<LineWidth>() > 0)
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> LineLayer::Impl::createBucket(const BucketParameters& parameters, const std::vector<const Layer*>& layers) const {
    return std::make_unique<LineBucket>(parameters, layers, layout);
}

float LineLayer::Impl::getLineWidth() const {
    float lineWidth = paint.evaluated.get<LineWidth>();
    float gapWidth = paint.evaluated.get<LineGapWidth>().constantOr(0);
    if (gapWidth) {
        return gapWidth + 2 * lineWidth;
    } else {
        return lineWidth;
    }
}

optional<GeometryCollection> offsetLine(const GeometryCollection& rings, const double offset) {
    if (offset == 0) return {};

    GeometryCollection newRings;
    Point<double> zero(0, 0);
    for (const auto& ring : rings) {
        newRings.emplace_back();
        auto& newRing = newRings.back();

        for (auto i = ring.begin(); i != ring.end(); i++) {
            auto& p = *i;

            Point<double> aToB = i == ring.begin() ?
                zero :
                util::perp(util::unit(convertPoint<double>(p - *(i - 1))));
            Point<double> bToC = i + 1 == ring.end() ?
                zero :
                util::perp(util::unit(convertPoint<double>(*(i + 1) - p)));
            Point<double> extrude = util::unit(aToB + bToC);

            const double cosHalfAngle = extrude.x * bToC.x + extrude.y * bToC.y;
            extrude *= (1.0 / cosHalfAngle);

            newRing.push_back(convertPoint<int16_t>(extrude * offset) + p);
        }
    }

    return newRings;
}

float LineLayer::Impl::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.evaluated.get<LineTranslate>();
    auto offset = paint.evaluated.get<LineOffset>().constantOr(LineOffset::defaultValue());
    return getLineWidth() / 2.0 + std::abs(offset) + util::length(translate[0], translate[1]);
}

bool LineLayer::Impl::queryIntersectsGeometry(
        const GeometryCoordinates& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    const float halfWidth = getLineWidth() / 2.0 * pixelsToTileUnits;

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.evaluated.get<LineTranslate>(), paint.evaluated.get<LineTranslateAnchor>(), bearing, pixelsToTileUnits);

    auto offset = paint.evaluated.get<LineOffset>().constantOr(LineOffset::defaultValue());
    auto offsetGeometry = offsetLine(geometry, offset * pixelsToTileUnits);

    return util::polygonIntersectsBufferedMultiLine(
            translatedQueryGeometry.value_or(queryGeometry),
            offsetGeometry.value_or(geometry),
            halfWidth);
}

} // namespace style
} // namespace mbgl
