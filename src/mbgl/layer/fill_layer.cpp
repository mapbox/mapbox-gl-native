#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/util/get_geometries.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> FillLayer::clone() const {
    return std::make_unique<FillLayer>(*this);
}

void FillLayer::parsePaints(const JSValue& layer) {
    paint.fillAntialias.parse("fill-antialias", layer);
    paint.fillOpacity.parse("fill-opacity", layer);
    paint.fillColor.parse("fill-color", layer);
    paint.fillOutlineColor.parse("fill-outline-color", layer);
    paint.fillTranslate.parse("fill-translate", layer);
    paint.fillTranslateAnchor.parse("fill-translate-anchor", layer);
    paint.fillPattern.parse("fill-pattern", layer);
}

void FillLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.fillAntialias.cascade(parameters);
    paint.fillOpacity.cascade(parameters);
    paint.fillColor.cascade(parameters);
    paint.fillOutlineColor.cascade(parameters);
    paint.fillTranslate.cascade(parameters);
    paint.fillTranslateAnchor.cascade(parameters);
    paint.fillPattern.cascade(parameters);
}

bool FillLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.fillAntialias.calculate(parameters);
    hasTransitions |= paint.fillOpacity.calculate(parameters);
    hasTransitions |= paint.fillColor.calculate(parameters);
    hasTransitions |= paint.fillOutlineColor.calculate(parameters);
    hasTransitions |= paint.fillTranslate.calculate(parameters);
    hasTransitions |= paint.fillTranslateAnchor.calculate(parameters);
    hasTransitions |= paint.fillPattern.calculate(parameters);

    passes = RenderPass::None;

    if (paint.fillAntialias) {
        passes |= RenderPass::Translucent;
    }

    if (!paint.fillPattern.value.from.empty() || (paint.fillColor.value[3] * paint.fillOpacity) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }

    return hasTransitions;
}

std::unique_ptr<Bucket> FillLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<FillBucket>();

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(getGeometries(feature));
    });

    return std::move(bucket);
}

} // namespace mbgl
