#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> CircleLayer::clone() const {
    std::unique_ptr<CircleLayer> result = std::make_unique<CircleLayer>();
    result->copy(*this);
    result->paint = paint;
    return std::move(result);
}

void CircleLayer::parsePaints(const JSVal& layer) {
    paint.radius.parse("circle-radius", layer);
    paint.color.parse("circle-color", layer);
    paint.opacity.parse("circle-opacity", layer);
    paint.translate.parse("circle-translate", layer);
    paint.translateAnchor.parse("circle-translate-anchor", layer);
    paint.blur.parse("circle-blur", layer);
}

void CircleLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.radius.cascade(parameters);
    paint.color.cascade(parameters);
    paint.opacity.cascade(parameters);
    paint.translate.cascade(parameters);
    paint.translateAnchor.cascade(parameters);
    paint.blur.cascade(parameters);
}

bool CircleLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.radius.calculate(parameters);
    hasTransitions |= paint.color.calculate(parameters);
    hasTransitions |= paint.opacity.calculate(parameters);
    hasTransitions |= paint.translate.calculate(parameters);
    hasTransitions |= paint.translateAnchor.calculate(parameters);
    hasTransitions |= paint.blur.calculate(parameters);

    passes = paint.isVisible() ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> CircleLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<CircleBucket>();

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(feature.getGeometries());
    });

    return std::move(bucket);
}

}
